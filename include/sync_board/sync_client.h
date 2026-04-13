

#pragma once

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdint>
#include <mutex>
#include "sync.pb.h"
#include <deque>
#include <iomanip>
#include <thread>

#define BASE_PORT 12080

class SyncClient
{
public:
    /**
     * @brief 构造函数，初始化客户端并连接到服务器.
     * @param server_ip 服务器IP地址，默认为 "127.0.0.1".
     * @param base_port 基础端口号，默认为 BASE_PORT.
     * 
     */
    SyncClient(const std::string &server_ip = "127.0.0.1", int base_port = BASE_PORT)
        : server_ip_(server_ip), control_sock_(-1), data_sock_(-1), data_port_(-1)
    {
        if (requestDataPort(base_port, true))
        {
            if (!connectDataPort())
            {
                std::cerr << "Failed to connect to data port\n";
            }
        }
        else
        {
            std::cerr << "Failed to request data port, Please check sync_core is running\n";
        }
    }

    ~SyncClient()
    {
        if (control_sock_ != -1)
            close(control_sock_);
        if (data_sock_ != -1)
            close(data_sock_);
    }
private:
    bool requestDataPort(int port, const bool is_reconnect = false)
    {
        do{
            control_sock_ = socket(AF_INET, SOCK_STREAM, 0);
            if (control_sock_ < 0)
            {
                std::cerr << "Socket creation failed\n";
                return false;
            }

            sockaddr_in serv_addr{};
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(BASE_PORT);
            serv_addr.sin_addr.s_addr = inet_addr(server_ip_.c_str());

            if (connect(control_sock_, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
            {
                std::cerr << "Connection failed, retrying...\n";
                close(control_sock_);
                control_sock_ = -1;
                sleep(2);
                if(!is_reconnect)
                    return false;
                continue;
            }
            std::cout << "Connected to control port\n";
            break;
        }while (is_reconnect);
        


        std::string cmd = "REQUEST_PORT " + std::to_string(port);
        send(control_sock_, cmd.c_str(), cmd.size(), 0);

        int res_port = -1;
        recv(control_sock_, &res_port, sizeof(res_port), 0);

        close(control_sock_);
        control_sock_ = -1;
        usleep(500000);

        if (res_port < 0)
        {
            std::cerr << "Failed to request port, please check sync_core is running\n";
            return false;
        }
        data_port_ = res_port;
        return true;
    }

    bool connectDataPort()
    {
        if (data_port_ < 0)
            return false;
        data_sock_ = socket(AF_INET, SOCK_STREAM, 0);
        if (data_sock_ < 0)
        {
            std::cerr << "Socket creation failed\n";
            return false;
        }

        sockaddr_in serv_addr{};
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(data_port_);
        serv_addr.sin_addr.s_addr = inet_addr(server_ip_.c_str());

        if (connect(data_sock_, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            std::cerr << "Connection to data_port failed\n";
            close(data_sock_);
            data_sock_ = -1;
            return false;
        }
        std::cout << "Connected to data port: " << data_port_ << "\n";
        return true;
    }
    void pushTriggerTimestamp(const uint64_t ref_recv_timestamp_us, const uint64_t trigger_timestamp_us)
    {
        std::lock_guard<std::mutex> lock(trigger_time_mutex_);
        if (trigger_time_queue_.size() > 10)
        {
            trigger_time_queue_.pop_front();
        }
        trigger_time_queue_.emplace_back(ref_recv_timestamp_us, trigger_timestamp_us);
    }

public:
    /**
     * @brief  同步板配置接口.
     * @param  prop_id 配置项ID.
     * @param  value  配置值.
     * @return bool  配置是否成功.
     */
    template <typename T>
    bool config(sync_proto::CfgPropID prop_id, const T &value)
    {
        if (data_sock_ < 0)
            return false;
        sync_proto::ConfigItem config_msg;
        config_msg.set_prop_id(prop_id);
        if constexpr (std::is_same<T, bool>::value)
        {
            config_msg.set_bool_value(value);
        }
        else if constexpr (std::is_same<T, int32_t>::value)
        {
            config_msg.set_int_value(value);
        }
        else if constexpr (std::is_same<T, float>::value)
        {
            config_msg.set_float_value(value);
        }
        else
        {
            std::cerr << "Unsupported config value type\n";
            return false;
        }

        std::string serialized_data;
        if (!config_msg.SerializeToString(&serialized_data))
        {
            std::cerr << "Failed to serialize config message\n";
            return false;
        }

        uint16_t magic = htons(0xA1CA);
        uint16_t len = htons(serialized_data.size());
        uint16_t id_u16 = htons(static_cast<uint16_t>(sync_proto::MessageID::MSG_CONFIG));

        std::string out;
        out.append(reinterpret_cast<char *>(&magic), sizeof(magic));
        out.append(reinterpret_cast<char *>(&len), sizeof(len));
        out.append(reinterpret_cast<char *>(&id_u16), sizeof(id_u16));
        out.append(serialized_data);

        send(data_sock_, out.c_str(), out.size(), 0);
        return true;
    }

    sync_proto::MessageID receiveLoopOnce(sync_proto::Envelope &msg)
    {
        if (data_sock_ < 0)
            return sync_proto::MessageID::MSG_UNKNOWN;
        size_t pos = 0;
        if (total_msg_str_.find("\xA1\xCA", pos) == std::string::npos)
        {
            char buffer[8912] = {0};
            int n = 0;
            // 数据太长就丢弃
            do{
                 n = recv(data_sock_, buffer, sizeof(buffer) - 1, 0);
            }while(n > 1024);

            if (n <= 0)
            {
                std::cout << "Connection closed by server or error occurred\n";
                close(data_sock_);
                data_sock_ = -1;
                total_msg_str_.clear();

                //reconnect
                requestDataPort(BASE_PORT, true);
                if (!connectDataPort())
                {
                    std::cerr << "Reconnection to data port failed\n";
                }
                std::cout << "Data socket reconnected\n";
                n = recv(data_sock_, buffer, sizeof(buffer) - 1, 0);

                // return sync_proto::MessageID::MSG_UNKNOWN;
            }
            total_msg_str_.append(buffer, n);
        }
        pos = 0;
        if ((pos = total_msg_str_.find("\xA1\xCA", pos)) != std::string::npos)
        {
            // 找到0xa1ca，处理后续逻辑
            pos += 2; // 跳过已找到的magic number
            uint16_t payload_len = (static_cast<uint8_t>(total_msg_str_[pos]) << 8) | static_cast<uint8_t>(total_msg_str_[pos + 1]);
            uint16_t id = (static_cast<uint8_t>(total_msg_str_[pos + 2]) << 8) | static_cast<uint8_t>(total_msg_str_[pos + 3]);
            pos += 4; // 跳过payload_len和id

            if (pos + payload_len > total_msg_str_.size())
            {
                std::cout << "Found message with ID: " << id << ", Payload Length: " << payload_len << std::endl;  
                std::cout << "total_msg_str_.size(): " << total_msg_str_.size() << ", pos: " << pos << std::endl;

                std::cout << "Hex dump of incomplete message: ";
                for (size_t i = 0; i < total_msg_str_.size(); ++i) {
                    std::cout << std::hex << std::setw(2) << std::setfill('0')
                              << (static_cast<unsigned int>(static_cast<unsigned char>(total_msg_str_[i]))) << " ";
                }
                std::cout << std::dec << std::endl;
                std::cerr << "Incomplete message received\n";
                total_msg_str_.erase(0, pos);
                return sync_proto::MessageID::MSG_UNKNOWN;
            }

            std::string msg_data = total_msg_str_.substr(pos, payload_len);
            pos += payload_len;
            // std::cout << std::fixed << std::setprecision(3) << "Current time (ms): " << now_us / 1000.0 << " Message id: " << id << std::endl;

            // 处理 msg_data，根据 id 解析不同的消息类型
            switch (id)
            {
            case sync_proto::MessageID::MSG_IMU_RAW:
            {
                sync_proto::IMU_RAW imu_msg;
                if (imu_msg.ParseFromString(msg_data))
                {
                    sync_proto::Envelope env;
                    env.set_id(sync_proto::MessageID::MSG_IMU_RAW);
                    env.set_allocated_imu_raw(new sync_proto::IMU_RAW(imu_msg));
                    msg = env;
                    total_msg_str_.erase(0, pos);
                    return sync_proto::MessageID::MSG_IMU_RAW;
                }
            }
            case sync_proto::MessageID::MSG_TRIGGER:
            {
                sync_proto::TRIGGER trigger_msg;
                if (trigger_msg.ParseFromString(msg_data))
                {
                    sync_proto::Envelope env;
                    env.set_id(sync_proto::MessageID::MSG_TRIGGER);
                    env.set_allocated_trigger(new sync_proto::TRIGGER(trigger_msg));
                    msg = env;
                    total_msg_str_.erase(0, pos);
                    pushTriggerTimestamp(trigger_msg.recv_trigger_time_us(), trigger_msg.trigger_time_us());
                    return sync_proto::MessageID::MSG_TRIGGER;
                }
            }
            case sync_proto::MessageID::MSG_LOG:
            {
                sync_proto::LOG log_msg;
                if (log_msg.ParseFromString(msg_data))
                {
                    sync_proto::Envelope env;
                    env.set_id(sync_proto::MessageID::MSG_LOG);
                    env.set_allocated_log(new sync_proto::LOG(log_msg));
                    msg = env;
                    total_msg_str_.erase(0, pos);
                    return sync_proto::MessageID::MSG_LOG;
                }
            }
            default:
                std::cerr << "Unknown message ID: " << id << std::endl;
                total_msg_str_.erase(0, pos);
                return sync_proto::MessageID::MSG_UNKNOWN;
                // break;
            }
        }
        return sync_proto::MessageID::MSG_UNKNOWN;
    }
    void receiveThreadStart()
    {
        recv_thread_ = std::thread([this]()
                                   {
            if (data_sock_ < 0)
            return;
            while (true)
            {
                sync_proto::Envelope msg;
                receiveLoopOnce(msg);
            }
            close(data_sock_);
            data_sock_ = -1;
        });
        recv_thread_.detach();
    }

    int getDataPort() const { return data_port_; }

    bool requestCameraTimestamp(const uint64_t ref_recv_timestamp_us, uint64_t &timestamp_us)
    {
        if (data_sock_ < 0)
        {
            std::cerr << "Data socket not connected\n";
            return false;
        }
        std::lock_guard<std::mutex> lock(trigger_time_mutex_);
        if (trigger_time_queue_.empty())
        {
            std::cerr << "No trigger timestamps available for comparison\n";
            return false;
        }
        auto best_pair = trigger_time_queue_.front();
        uint64_t best_diff = std::abs((int64_t)(ref_recv_timestamp_us - best_pair.first));
        for (const auto &pair : trigger_time_queue_)
        {
            uint64_t diff = std::abs((int64_t)(ref_recv_timestamp_us - pair.first));
            if (diff < best_diff)
            {
                best_diff = diff;
                best_pair = pair;
            }
        }
        timestamp_us = best_pair.second;
        return true;
    }


private:
    std::string server_ip_;
    int control_sock_;
    int data_sock_;
    int data_port_;
    std::deque<std::pair<uint64_t, uint64_t>> trigger_time_queue_; // <recv_time_us, trigger_time_us>
    std::mutex trigger_time_mutex_;
    std::string total_msg_str_;
    std::thread recv_thread_;
};
