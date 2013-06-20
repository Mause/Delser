namespace exceptions {
    class key_gen_exception {};
    

    class key_invalid             : public key_gen_exception, public std::runtime_error {
        public:
            template<typename T>
            explicit key_invalid(T message) : std::runtime_error(message), msg_(message) {}
            virtual ~key_invalid() throw (){}
            virtual const char* what() const throw (){ return msg_.c_str(); }
        protected: std::string msg_;
    };
    class key_blacklisted         : public key_gen_exception, public std::runtime_error {
        public:
            template<typename T>
            explicit key_blacklisted(T message) : std::runtime_error(message), msg_(message) {}
            virtual ~key_blacklisted() throw (){}
            virtual const char* what() const throw (){ return msg_.c_str(); }
        protected: std::string msg_;
    };
    class key_phony               : public key_gen_exception, public std::runtime_error {
        public:
            template<typename T>
            explicit key_phony(T message) : std::runtime_error(message), msg_(message) {}
            virtual ~key_phony() throw (){}
            virtual const char* what() const throw (){ return msg_.c_str(); }
        protected: std::string msg_;
    };
    class key_bad_checksum        : public key_gen_exception, public std::runtime_error {
        public:
            template<typename T>
            explicit key_bad_checksum(T message) : std::runtime_error(message), msg_(message) {}
            virtual ~key_bad_checksum() throw (){}
            virtual const char* what() const throw (){ return msg_.c_str(); }
        protected: std::string msg_;
    };

    class delser_internal_exception {};
    class bad_byte_to_check_error : public delser_internal_exception, public std::runtime_error {
        public:
            template<typename T>
            explicit bad_byte_to_check_error(T message) : std::runtime_error(message), msg_(message) {} 
            virtual ~bad_byte_to_check_error() throw (){}
            virtual const char* what() const throw (){ return msg_.c_str(); }
        protected: std::string msg_;
    };
};
