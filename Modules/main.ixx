module;
export module main;

import <cstdint>;
import <vector>;
import <random>;
import <string>;
import <string_view>;
import <iostream>;
import <type_traits>;
import <optional>;
import <concepts>;
import <iterator>;
import <memory>;
import <fstream>;
import <vulkan/vulkan.hpp>;

struct VulkanVersion {
	std::uint32_t patch : 12;
	std::uint32_t minor : 10;
	std::uint32_t major : 10;

	constexpr VulkanVersion(const std::uint32_t version) noexcept :
		patch{ version & 0xfff },
		minor{ (version >> 12) & 0x3ff },
		major{ version >> 22 } {}

	constexpr VulkanVersion(
		const std::uint32_t major,
		const std::uint32_t minor,
		const std::uint32_t patch) noexcept :
		patch{ major },
		minor{ minor },
		major{ patch } {}

	explicit constexpr operator std::uint32_t() const {
		return (major << 22) | (minor << 12) | patch;
	}
};

using namespace std::string_literals;

export int main() {
	auto loader = vk::DynamicLoader{};
	auto table = vk::DispatchLoaderDynamic{
		loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr") };

	const auto exts = vk::enumerateInstanceExtensionProperties(nullptr, table);
	const auto layers = vk::enumerateInstanceLayerProperties(table);
	const auto ver = static_cast<VulkanVersion>(vk::enumerateInstanceVersion(table));

	std::cout << ver.major << "." << ver.minor << "." << ver.patch << '\n';
	std::cout << '\n';

	for (const auto& ext : exts) {
		std::cout << ext.extensionName << '\n';
	}

	std::cout << '\n';

	for (const auto& layer : layers) {
		std::cout << layer.layerName << '\n';
	}

	std::cout << '\n';

	const auto applicationInfo = vk::ApplicationInfo{
		"test.exe",
		0,
		"Engine",
		0,
		static_cast<std::uint32_t>(VulkanVersion{ 1, 2, 0 }) };
	const auto createInfo = vk::InstanceCreateInfo{ vk::InstanceCreateFlags{}, &applicationInfo };

	auto inst = vk::createInstance(createInfo, nullptr, table);
	table.init(inst);

	const auto devs = inst.enumeratePhysicalDevices(table);
	const auto dev_exts = devs.front().enumerateDeviceExtensionProperties(nullptr, table);
	for (const auto& ext : dev_exts) {
		std::cout << ext.extensionName << '\n';
	}

	inst.destroy(nullptr, table);
}

