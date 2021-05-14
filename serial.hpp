#pragma once

#include <iostream>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <thread>

class Serial {

    public:
        typedef std::function <void(const std::string, const size_t size)> callback_func;
    public:

        Serial():io(), serial(io), readbuffer(), _read_callback(), read_loop(), _debug(){}

        Serial(const bool debug_mode):io(), serial(io), readbuffer(), _read_callback(), read_loop(), _debug(debug_mode){}

        bool open(const std::string port, const uint32_t baud, \
                boost::asio::serial_port_base::parity parity = \
                boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none), \
                boost::asio::serial_port_base::character_size char_size = \
                boost::asio::serial_port_base::character_size(boost::asio::serial_port_base::character_size(8)), \
                boost::asio::serial_port_base::flow_control flow_ctrl = \
                boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none), \
                boost::asio::serial_port_base::stop_bits stop_bits = \
                boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one)){

            if(serial.is_open()){

                std::cout<<port<<" is already open"<<std::endl;
                return false;
            }

            else{

                serial.open(port);
                serial.set_option(boost::asio::serial_port_base::baud_rate(baud));
                serial.set_option(parity);
                serial.set_option(char_size);
                serial.set_option(flow_ctrl);
                serial.set_option(stop_bits);

                std::cout<<port<<" is now open"<<std::endl;
            }

            return true;
        }

        void set_callback(const callback_func &_callback){
            _read_callback = _callback;
        }

        void async_write(const std::string data){
            boost::asio::async_write(serial, boost::asio::buffer(data), boost::bind(&Serial::sent, this, \
                boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }

        void async_write(const char* data, uint16_t size){
            boost::asio::async_write(serial, boost::asio::buffer(data, size), boost::bind(&Serial::sent, this, \
                boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }

        void async_write(const std::vector<uint8_t> data){
            boost::asio::async_write(serial, boost::asio::buffer(data), boost::bind(&Serial::sent, this, \
                boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }

        void async_write(const char data){
            boost::asio::async_write(serial, boost::asio::buffer(&data, 1), boost::bind(&Serial::sent, this, \
                boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }

        uint8_t write(const std::string data){
            return boost::asio::write(serial, boost::asio::buffer(data), lErrorCode);
        }

        uint8_t write(const char* data, uint16_t size){
            return boost::asio::write(serial, boost::asio::buffer(data, size), lErrorCode);
        }

        uint8_t write(const std::vector<uint8_t> data){
            return boost::asio::write(serial, boost::asio::buffer(data), lErrorCode);
        }

        void async_read_until(void){
            boost::asio::async_read_until(serial, readbuffer, "\r\n", boost::bind(&Serial::received, this, \
                boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }

        void async_read_until(const bool loop_mode){
            read_loop = loop_mode;
            boost::asio::async_read_until(serial, readbuffer, "\r\n", boost::bind(&Serial::received, this, \
                boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }

        uint8_t read(char* readbufferer, uint8_t size){
            return boost::asio::read(serial, boost::asio::buffer(readbufferer,size), lErrorCode);
        }

        void received(const boost::system::error_code& e, std::size_t size){
         if (!e){
                std::istream is(&readbuffer);
                std::string line;
                std::getline(is, line);
                _read_callback(line, size);
            }
            else{
                std::cout<<e.message()<<std::endl;
                exit(0);
            }
            readbuffer.consume(size);
            if(read_loop)
                async_read_until();
        }

        void sent(const boost::system::error_code& e, std::size_t size){
            if (!e){
                std::cout<<"Data sent!"<<std::endl;
            }
            else{
                std::cout<<e.message()<<std::endl;
            }
        }

        void close(){
            serial.cancel();
            read_loop = false;
            serial.close();
            io.stop();
            io.reset();
        }

        virtual ~Serial(){}

    public:
        boost::asio::io_service io;
    private:
        boost::asio::streambuf readbuffer;
        boost::system::error_code lErrorCode;
        boost::asio::serial_port serial;
        callback_func _read_callback;
        bool read_loop;
        bool _debug;
};
