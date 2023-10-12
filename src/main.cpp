#include "Hooks.h"

void MessageHandler(F4SE::MessagingInterface::Message* a_message)
{
	switch (a_message->type) {
	case F4SE::MessagingInterface::kPostLoad:
		SimpleOffenceSuppression::InstallOnPostLoad();
		break;
	case F4SE::MessagingInterface::kGameDataReady:
		{
			const auto settings = Settings::GetSingleton();

			if (settings->changeGameSettings) {
				constexpr auto set_gmst = [](std::string_view a_name, std::int32_t a_value) {
					if (const auto gmst = RE::GameSettingCollection::GetSingleton()->GetSetting(a_name)) {
						gmst->SetInt(a_value);
					}
					logger::info("Gamesetting : {} : {}", a_name, a_value);
				};

				set_gmst("iFriendHitNonCombatAllowed", settings->iFriendHitNonCombatAllowed);
				set_gmst("iFriendHitCombatAllowed", settings->iFriendHitCombatAllowed);
				set_gmst("iAllyHitNonCombatAllowed", settings->iAllyHitNonCombatAllowed);
				set_gmst("iAllyHitCombatAllowed", settings->iAllyHitCombatAllowed);
			}

		    SimpleOffenceSuppression::actorTypeAnimalKYWD = RE::TESForm::GetFormByID<RE::BGSKeyword>(0x00013798);
			logger::info("ActorTypeAnimal keyword lookup {}", SimpleOffenceSuppression::actorTypeAnimalKYWD != nullptr ? "succeeded" : "failed");

			SimpleOffenceSuppression::actorTypeCreatureKYWD = RE::TESForm::GetFormByID<RE::BGSKeyword>(0x00013795);
			logger::info("ActorTypeCreature keyword lookup {}", SimpleOffenceSuppression::actorTypeCreatureKYWD != nullptr ? "succeeded" : "failed");
		}
		break;
	default:
		break;
	}
}

void InitializeLog()
{
	auto path = logger::log_directory();
	if (!path) {
		stl::report_and_fail("Failed to find standard logging directory"sv);
	}

	*path /= Version::PROJECT;
	*path += ".log"sv;
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%H:%M:%S:%e] %v"s);

	logger::info(FMT_STRING("{} v{}"), Version::PROJECT, Version::NAME);
}

extern "C" DLLEXPORT bool F4SEAPI F4SEPlugin_Query(const F4SE::QueryInterface* a_F4SE, F4SE::PluginInfo* a_info)
{
	a_info->infoVersion = F4SE::PluginInfo::kVersion;
	a_info->name = Version::PROJECT.data();
	a_info->version = Version::MAJOR;

	const auto ver = a_F4SE->RuntimeVersion();
	if (ver < F4SE::RUNTIME_LATEST) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}

extern "C" DLLEXPORT bool F4SEAPI F4SEPlugin_Load(const F4SE::LoadInterface* a_f4se)
{
	InitializeLog();

	F4SE::Init(a_f4se);

	const auto messaging = F4SE::GetMessagingInterface();
	messaging->RegisterListener(MessageHandler);

	return true;
}
