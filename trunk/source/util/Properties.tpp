/***
 * hesperus: Properties.tpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include <source/exceptions/Exception.h>

namespace hesp {

//#################### PUBLIC METHODS ####################
template <typename T>
shared_ptr<T> Properties::get(const std::string& name) const
{
	if(!has(name)) return shared_ptr<T>();

	try
	{
		return boost::any_cast<shared_ptr<T> >(m_properties.find(name)->second);
	}
	catch(boost::bad_any_cast&)
	{
		throw Exception("The property " + name + " does not have the specified type");
	}
}

template <typename T>
const T& Properties::get_actual(const std::string& name) const
{
	shared_ptr<T> p = get<T>(name);
	if(p) return *p;
	else throw Exception("Missing property: " + name);
}

template <typename T>
void Properties::set(const std::string& name, const shared_ptr<T>& value)
{
	m_properties[name] = value;
}

template <typename T>
void Properties::set_actual(const std::string& name, const T& value)
{
	m_properties[name] = shared_ptr<T>(new T(value));
}

}
