#include "memory.h"

#include <thread>

namespace offsets
{
	//client
	constexpr ::std::ptrdiff_t dwLocalPlayer = 0xDEA964;
	constexpr ::std::ptrdiff_t dwEntityList = 0x4DFFF14;

	//engine
	constexpr ::std::ptrdiff_t model_ambient_min = 0x5A118C;

	//entity
	constexpr ::std::ptrdiff_t m_clrRender = 0x70;
	constexpr ::std::ptrdiff_t m_iTeamNum = 0xF4;
}

struct Color
{
	std::uint8_t r{ }, g{ }, b{ };
};

int main()
{
	const auto memory = Memory{ "csgo.exe" };

	const auto client = memory.GetModuleAddress("client.dll");
	const auto engine = memory.GetModuleAddress("engine.dll");

	constexpr const auto teamColor = Color{ 0,0,255 };
	constexpr const auto enemyColor = Color{ 255,0,0 };

	//loop
	while (true)
	{
		const auto& localplayer = memory.Read<std::uintptr_t>(client + offsets::dwLocalPlayer);
		const auto& localTeam = memory.Read<std::int32_t>(localplayer + offsets::m_iTeamNum);

		for (auto i = 1; i <= 32; ++i)
		{
			const auto& entity = memory.Read<std::uintptr_t>(client + offsets::dwEntityList + i * 0x10);

			if (memory.Read<std::int32_t>(entity + offsets::m_iTeamNum) == localTeam)
			{
				memory.Write<Color>(entity + offsets::m_clrRender, teamColor);
			}
			else
			{
				memory.Write<Color>(entity + offsets::m_clrRender, enemyColor);
			}

			//model brightness
			float brightness = 25.f;
			const auto _this = static_cast<std::uintptr_t>(entity + offsets::model_ambient_min - 0x2c);
			memory.Write<std::int32_t>(entity + offsets::model_ambient_min, *reinterpret_cast<std::uintptr_t*>(&brightness) * _this);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	return 0;
}