#ifndef __STD_EX_HPP
#define __STD_EX_HPP



#include <functional>



namespace stdex
{


	template<typename OperatorT1, typename OperatorT2, typename OperatorT3>
	class BinaryCompose
		: public std::unary_function<typename OperatorT2::argument_type,
		typename OperatorT1::result_type>
	{
	public:
		typedef typename OperatorT1::result_type	result_type;
		typedef typename OperatorT2::argument_type	argument_type;

	private:
		OperatorT1 m_func1;
		OperatorT2 m_func2;
		OperatorT3 m_func3;

	public:
		BinaryCompose(const OperatorT1 &x, const OperatorT2 &y, const OperatorT3 &z)
			: m_func1(x)
			, m_func2(y)
			, m_func3(z)
		{
		}

		result_type operator()(const argument_type &x) const
		{
			return m_func1(m_func2(x), m_func3(x));
		}
	};

	template<typename OperatorT1, typename OperatorT2, typename OperatorT3>
	inline BinaryCompose<OperatorT1, OperatorT2, OperatorT3> Compose2(const OperatorT1 &func1, 
		const OperatorT2 &func2, const OperatorT3 &func3)
	{
		return BinaryCompose<OperatorT1, OperatorT2, OperatorT3>(func1, func2, func3);
	}



}

#endif