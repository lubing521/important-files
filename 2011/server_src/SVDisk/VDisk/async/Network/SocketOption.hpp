#ifndef __SOCKET_SOCKET_OPTION_HPP
#define __SOCKET_SOCKET_OPTION_HPP



namespace async
{
	namespace network
	{




		// ----------------------------------------------
		// 基于bool设置socket属性的帮助类

		template<int _Level, int _Name>
		class Boolean
		{
		private:
			int value_;

		public:
			Boolean()
				: value_(0)
			{}
			explicit Boolean(bool val)
				: value_(val ? 1 : 0)
			{}
			Boolean &operator=(const Boolean &rhs)
			{
				value_	 = rhs.value_;
				return *this;
			}	
			Boolean &operator=(bool val)
			{
				value_ = val ? 1 : 0;
			}

		public:
			bool value() const
			{
				return !!value_;
			}

			// bool 转换
			operator bool() const
			{
				return !!value_;
			}

			bool operator!() const
			{
				return !value_;
			}

			// 获取Socket leval选项值
			int level() const
			{
				return _Level;
			}

			// 获取Socket name
			int name() const
			{
				return _Name;
			}	

			// 获取Socket值
			char *data()
			{
				return &value_;
			}
			const char *data() const
			{
				return &value_;
			}

			// 获取值大小
			size_t size() const
			{
				return sizeof(value_);
			}


			// 设置boolean的大小
			void resize(size_t nSize)
			{
				// 在某些平台，getsockopt返回一个sizeof(bool)--1字节。	
				// 
				switch(nSize)
				{
				case sizeof(char):
					value_ = *reinterpret_cast<char *>(&value_) ? 1 : 0;
					break;
				case sizeof(value_):
					break;
				default:
					throw std::length_error("bool socket opetion resize");
				}
			}
		};


		// ----------------------------------------------------------
		// 基于int设置socket属性的帮助类

		template<int _Level, int _Name>
		class Integer
		{
		private:
			int value_;

		public:
			Integer()
				: value_(0)
			{}
			explicit Integer(int val)
				: value_(val)
			{}
			Integer &operator=(const Integer &rhs)
			{
				value_	 = rhs.value_;
				return *this;
			}	
			Integer &operator=(int val)
			{
				value_ = val;
			}

		public:
			bool value() const
			{
				return !!value_;
			}

			// 获取Socket leval选项值
			int level() const
			{
				return _Level;
			}

			// 获取Socket name
			int name() const
			{
				return _Name;
			}	

			// 获取Socket值
			char *data()
			{
				return reinterpret_cast<char *>(&value_);
			}
			const char *data() const
			{
				return (const char *)&value_;
			}

			// 获取值大小
			size_t size() const
			{
				return sizeof(value_);
			}


			// 设置int的大小
			void resize(size_t nSize)
			{
				if( nSize != sizeof(value_) )
					throw std::length_error("Integer socket option resize");
			}
		};


		// -----------------------------------------------------------------
		// class Linger 选项设置帮助类

		// SO_LINGER用于控制当未发送的数据在套接字上排队时，一旦执行了closesocket后，该采取何种动作
		// 如果超出延时，则未发送和接受的数据都会丢弃，同时重设对方的连接
		// 也可以通过设置SO_DONTLINGER来设置

		template<int _Level, int _Name>
		class Linger
		{
		private:
			::linger value_;


		public:
			Linger()
			{
				value_.l_linger = 0;	// option on/off
				value_.l_onoff	= 0;	// linger time
			}
			Linger(bool bOn, int nTime)
			{
				enabled(bOn);
				timeout(nTime);
			}


		public:
			// 打开或关闭linger选项
			void enabled(bool val)
			{
				value_.l_onoff = val ? 1 : 0;
			}

			bool enabled() const
			{
				return value_.l_onoff != 0;
			}

			void timeout(int val)
			{
				value_.l_linger = static_cast<u_short>(val);
			}
			int timeout() const
			{
				return static_cast<int>(value_.l_linger);
			}

			// 获取Socket leval选项值
			int level() const
			{
				return _Level;
			}

			// 获取Socket name
			int name() const
			{
				return _Name;
			}	

			// 获取Socket值
			::linger *data()
			{
				return &value_;
			}
			const ::linger *data() const
			{
				return &value_;
			}

			// 获取值大小
			size_t size() const
			{
				return sizeof(value_);
			}

			// 设置int的大小
			void resize(size_t nSize)
			{
				if( nSize != sizeof(value_) )
					throw std::length_error("Linger socket option resize");
			}

		};






		typedef u_long IoctlArgType;

		// ---------------------------------------------------
		// class NonBlockingIO

		class NonBlockingIO
		{
		private:
			IoctlArgType value_;

		public:
			NonBlockingIO()
				: value_(0)
			{}
			NonBlockingIO(bool value)
				: value_(value ? 1 : 0)
			{}

		public:
			int name() const
			{
				return FIONBIO;
			}

			void set(bool value)
			{
				value_ = value ? 1 : 0;
			}
			bool get() const
			{
				return value_ != 0;
			}

			IoctlArgType *data()
			{
				return &value_;
			}

			const IoctlArgType *data() const
			{
				return &value_;
			}
		};


		// ---------------------------------------------------
		// class BytesReadable

		class BytesReadable
		{
		private:
			IoctlArgType value_;

		public:
			BytesReadable()
				: value_(0)
			{}
			BytesReadable(bool value)
				: value_(value ? 1 : 0)
			{}

		public:
			int name() const
			{
				return FIONREAD;
			}

			void set(size_t value)
			{
				value_ = static_cast<IoctlArgType>(value);
			}
			size_t get() const
			{
				return static_cast<size_t>(value_);
			}

			IoctlArgType *data()
			{
				return &value_;
			}

			const IoctlArgType *data() const
			{
				return &value_;
			}
		};


		//----------------------------------

		/*
			Example:

				tcp::socket sock(io);
				SocketBase::Broadcast opt(true);
				socket.SetOption(opt);
			*/

			typedef Boolean<SOL_SOCKET, SO_BROADCAST>	Broadcast;
			typedef Boolean<SOL_SOCKET, SO_DEBUG>		Debug;
			typedef Boolean<SOL_SOCKET, SO_DONTROUTE>	DontRoute;
			typedef Boolean<SOL_SOCKET, SO_KEEPALIVE>	KeepAlive;
			typedef Boolean<SOL_SOCKET, SO_REUSEADDR>	ReuseAddr;

			typedef Integer<SOL_SOCKET, SO_SNDBUF>		SendBufSize;
			typedef Integer<SOL_SOCKET, SO_SNDLOWAT>	SendLowWaterMark;
			typedef Integer<SOL_SOCKET, SO_RCVBUF>		RecvBufSize;
			typedef Integer<SOL_SOCKET, SO_RCVLOWAT>	RecvLowWwaterMark;
			//typedef Integer<SOL_SOCKET, SO_LINGER>		Linger;
			typedef Integer<SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT> UpdateAcceptContext;


			typedef NonBlockingIO						NonBlockingIO;
			typedef BytesReadable						BytesReadable;
	}
}





#endif