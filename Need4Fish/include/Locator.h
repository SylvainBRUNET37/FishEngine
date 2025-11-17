#ifndef LOCATOR_H
#define LOCATOR_H

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>

// Contains global classes
class Locator
{
public:
	template <typename Service>
	static void Set(const std::shared_ptr<Service>& service)
	{
		services[std::type_index(typeid(Service))] = service;
	}

	template <typename Service>
	static Service& Get()
	{
		const auto it = services.find(std::type_index(typeid(Service)));
		if (it == services.end())
			throw std::runtime_error("Service not found. It may hasn't been set in the locator.");

		return *std::static_pointer_cast<Service>(it->second);
	}

private:
	static inline std::unordered_map<std::type_index, std::shared_ptr<void>> services;
};

#endif
