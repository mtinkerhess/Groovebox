#pragma once

#include "json.hpp"
#include "MyMath.h"
#include "Voice/SynthParamGetters.h"
#include "Voice/SynthVoiceParam.h"
#include "Voice/SynthVoiceValueParams.h"

#include <functional>
#include <map>
#include <string>

namespace SynthParamReflection
{
	class IValue
	{
	public:
		virtual const std::string& GetName() const = 0;
		virtual const std::string& GetShortName() const = 0;
		virtual short GetMin() const = 0;
		virtual short GetMax() const = 0;
		virtual short GetInitial() const = 0;
		virtual double GetDoubleMin() const = 0;
		virtual double GetDoubleMax() const = 0;
		virtual short ClampValue(short value) = 0;
		virtual double ClampDoubleValue(double value) = 0;
	};

	template <
		typename SynthVoiceParamsCategoryType,
		typename SynthVoiceOptionalParamsCategoryType,
		typename SynthVoiceModulationShortParamsCategoryType>
	class Value : public IValue
	{
	private:
		std::string m_name;
		std::string m_shortName;
		short m_min;
		short m_max;
		short m_initial;
		double m_doubleMin;
		double m_doubleMax;
		std::function<const SynthVoiceParam& (const SynthVoiceParamsCategoryType&)> m_synthVoiceParamGetter;
		std::function<const SynthVoiceOptionalParam& (const SynthVoiceOptionalParamsCategoryType&)> m_synthVoiceOptionalParamGetter;
		std::function<const short& (const SynthVoiceModulationShortParamsCategoryType&)> m_synthVoiceModulationShortParamGetter;

	public:
		Value(
			const std::string& name,
			const std::string& shortName,
			short min,
			short max,
			short initial,
			double doubleMin,
			double doubleMax,
			std::function<const SynthVoiceParam& (const SynthVoiceParamsCategoryType&)> synthVoiceParamGetter,
			std::function<const SynthVoiceOptionalParam& (const SynthVoiceOptionalParamsCategoryType&)> synthVoiceOptionalParamGetter,
			std::function<const short& (const SynthVoiceModulationShortParamsCategoryType&)> synthVoiceModulationShortParamGetter)
			:
			m_name(name),
			m_shortName(shortName),
			m_min(min),
			m_max(max),
			m_initial(initial),
			m_doubleMin(doubleMin),
			m_doubleMax(doubleMax),
			m_synthVoiceParamGetter(synthVoiceParamGetter),
			m_synthVoiceOptionalParamGetter(synthVoiceOptionalParamGetter),
			m_synthVoiceModulationShortParamGetter(synthVoiceModulationShortParamGetter)
		{
		}

		const std::string& GetName() const override { return m_name; }
		const std::string& GetShortName() const override { return m_shortName; }
		short GetMin() const override { return m_min; }
		short GetMax() const override { return m_max; }
		short GetInitial() const override { return m_initial; }
		double GetDoubleMin() const override { return m_doubleMin; }
		double GetDoubleMax() const override { return m_doubleMax; }

		short ClampValue(short value) override { return Math::Clamp(value, m_min, m_max); }
		double ClampDoubleValue(double value) override { return Math::Clamp(value, m_doubleMin, m_doubleMax); }

		const SynthVoiceParam& GetSynthVoiceParamConst(const SynthVoiceParamsCategoryType& synthVoiceParams) const { return m_synthVoiceParamGetter(synthVoiceParams); }
		SynthVoiceParam& GetSynthVoiceParam(SynthVoiceParamsCategoryType& synthVoiceParams) const { return const_cast<SynthVoiceParam&>(m_synthVoiceParamGetter(synthVoiceParams)); }

		const SynthVoiceOptionalParam& GetSynthVoiceOptionalParamConst(const SynthVoiceOptionalParamsCategoryType& synthVoiceParams) const { return m_synthVoiceOptionalParamGetter(synthVoiceParams); }
		SynthVoiceOptionalParam& GetSynthVoiceOptionalParam(SynthVoiceOptionalParamsCategoryType& synthVoiceParams) const { return const_cast<SynthVoiceOptionalParam&>(m_synthVoiceOptionalParamGetter(synthVoiceParams)); }

		const short& GetSynthVoiceModulationShortParamConst(const SynthVoiceModulationShortParamsCategoryType& synthVoiceParams) const { return m_synthVoiceModulationShortParamGetter(synthVoiceParams); }
		short& GetSynthVoiceModulationShortParam(SynthVoiceModulationShortParamsCategoryType& synthVoiceParams) const { return const_cast<short&>(m_synthVoiceModulationShortParamGetter(synthVoiceParams)); }
	};

	class ICategory
	{
	public:
		virtual nlohmann::json ToJson(const SynthVoiceParams& params) const = 0;
		virtual void LoadFromJson(const nlohmann::json& json, const SynthVoiceParams& params) const = 0;
		virtual const std::vector<IValue*>& GetValues() const = 0;
		virtual const std::string& GetName() const = 0;
	};

	template<
		typename SynthVoiceParamsCategoryType,
		typename SynthVoiceOptionalParamsCategoryType,
		typename SynthVoiceModulationShortParamsCategoryType>
	class Category : public ICategory
	{
	public:
		using CategoryType = SynthVoiceParamsCategoryType;
		using OptionalCategoryType = SynthVoiceOptionalParamsCategoryType;
		using ModulationShortCategoryType = SynthVoiceModulationShortParamsCategoryType;

		using ValueType = Value<SynthVoiceParamsCategoryType, SynthVoiceOptionalParamsCategoryType, SynthVoiceModulationShortParamsCategoryType>;
		using SynthVoiceParamsGategoryGetter = std::function<const SynthVoiceParamsCategoryType& (const SynthVoiceParams&)>;
		using SynthVoiceOptionalParamsCategoryGetter = std::function<const SynthVoiceOptionalParamsCategoryType& (const SynthVoiceOptionalParams&)>;
		using SynthVoiceModulationShortParamsCategoryGetter = std::function<const SynthVoiceModulationShortParamsCategoryType& (const SynthVoiceModulationShortParams&)>;

	private:
		std::string m_name;
		SynthVoiceParamsGategoryGetter m_paramsCategoryGetter;
		SynthVoiceOptionalParamsCategoryGetter m_optionalParamsCategoryGetter;
		SynthVoiceModulationShortParamsCategoryGetter m_modulationShortParamsCategoryGetter;

	protected:
		std::vector<IValue*> m_values;

		Category(
			const std::string& name,
			SynthVoiceParamsGategoryGetter paramsCategoryGetter,
			SynthVoiceOptionalParamsCategoryGetter optionalParamsCategoryGetter,
			SynthVoiceModulationShortParamsCategoryGetter modulationShortParamsCategoryGetter)
			:
			m_name(name),
			m_paramsCategoryGetter(paramsCategoryGetter),
			m_optionalParamsCategoryGetter(optionalParamsCategoryGetter),
			m_modulationShortParamsCategoryGetter(modulationShortParamsCategoryGetter)
		{
		}

	public:
		const std::vector<IValue*>& GetValues() const override { return m_values; }
		const std::string& GetName() const override { return m_name; }

		nlohmann::json ToJson(const SynthVoiceParams& params) const override
		{
			const SynthVoiceParamsCategoryType& paramsCategory = m_paramsCategoryGetter(params);

			nlohmann::json json;

			for (const IValue* value : m_values)
			{
				const ValueType* valueType = static_cast<const ValueType*>(value);
				const SynthVoiceParam& voiceParam = valueType->GetSynthVoiceParamConst(paramsCategory);
				json[value->GetName()] = voiceParam.Value();
			}

			return json;
		}

		void LoadFromJson(const nlohmann::json& json, const SynthVoiceParams& params) const override
		{
			SynthVoiceParamsCategoryType& paramsCategory = const_cast<SynthVoiceParamsCategoryType&>(m_paramsCategoryGetter(params));

			for (const IValue* value : m_values)
			{
				const ValueType* valueType = static_cast<const ValueType*>(value);
				SynthVoiceParam& voiceParam = valueType->GetSynthVoiceParam(paramsCategory);
				voiceParam.SetValue(json[value->GetName()]);
			}
		}

		const SynthVoiceParamsCategoryType& GetParamsCategoryConst(const SynthVoiceParams& params) const { return m_paramsCategoryGetter(params); }
		SynthVoiceParamsCategoryType& GetParamsCategory(SynthVoiceParams& params) const { return const_cast<SynthVoiceParamsCategoryType&>(m_paramsCategoryGetter(params)); }

		const SynthVoiceOptionalParamsCategoryType& GetOptionalParamsCategoryConst(const SynthVoiceOptionalParams& params) const { return m_optionalParamsCategoryGetter(params); }
		SynthVoiceOptionalParamsCategoryType& GetOptionalParamsCategory(SynthVoiceOptionalParams& params) const { return const_cast<SynthVoiceOptionalParamsCategoryType&>(m_optionalParamsCategoryGetter(params)); }

		const SynthVoiceModulationShortParamsCategoryType& GetModulationShortParamsCategoryConst(const SynthVoiceModulationShortParams& params) const { return m_modulationShortParamsCategoryGetter(params); }
		SynthVoiceModulationShortParamsCategoryType& GetModulationShortParamsCategory(SynthVoiceModulationShortParams& params) const { return const_cast<SynthVoiceModulationShortParamsCategoryType&>(m_modulationShortParamsCategoryGetter(params)); }
	};

#define VALUE(categoryType, name, shortName, min, max, initial, doubleMin, doubleMax) \
private: \
Value<SynthVoiceParams::categoryType, SynthVoiceOptionalParams::categoryType, SynthVoiceModulationShortParams::categoryType> name \
{#name, #shortName, min, max, initial, doubleMin, doubleMax, \
[](const SynthVoiceParams::categoryType& category) -> const SynthVoiceParam& { return category.Get##name(); }, \
[](const SynthVoiceOptionalParams::categoryType& category) -> const SynthVoiceOptionalParam& { return category.Get##name(); }, \
[](const SynthVoiceModulationShortParams::categoryType& category) -> const short& { return category.name; } }; \
public: const Value<SynthVoiceParams::categoryType, SynthVoiceOptionalParams::categoryType, SynthVoiceModulationShortParams::categoryType>& Get##name() const { return name; }

#define CLASS_NAME SynthCategory
#define PARAMS_TYPE_NAME SynthParamsType
#define MACROS_PATH "Voice/Macros/SynthParamMacros.h"
#include "Voice/Templates/ReflectionCategoryTemplate.h"
#undef CLASS_NAME
#undef PARAMS_TYPE_NAME
#undef MACROS_PATH

#define CLASS_NAME OscillatorCategory
#define PARAMS_TYPE_NAME OscillatorParamsType
#define MACROS_PATH "Voice/Macros/OscillatorParamMacros.h"
#include "Voice/Templates/ReflectionCategoryTemplate.h"
#undef CLASS_NAME
#undef PARAMS_TYPE_NAME
#undef MACROS_PATH

#define CLASS_NAME EnvelopeCategory
#define PARAMS_TYPE_NAME EnvelopeParamsType
#define MACROS_PATH "Voice/Macros/EnvelopeParamMacros.h"
#include "Voice/Templates/ReflectionCategoryTemplate.h"
#undef CLASS_NAME
#undef PARAMS_TYPE_NAME
#undef MACROS_PATH

#define CLASS_NAME LfoCategory
#define PARAMS_TYPE_NAME LfoParamsType
#define MACROS_PATH "Voice/Macros/LfoParamMacros.h"
#include "Voice/Templates/ReflectionCategoryTemplate.h"
#undef CLASS_NAME
#undef PARAMS_TYPE_NAME
#undef MACROS_PATH

#define CLASS_NAME FilterCategory
#define PARAMS_TYPE_NAME FilterParamsType
#define MACROS_PATH "Voice/Macros/FilterParamMacros.h"
#include "Voice/Templates/ReflectionCategoryTemplate.h"
#undef CLASS_NAME
#undef PARAMS_TYPE_NAME
#undef MACROS_PATH

#define CLASS_NAME DelayCategory
#define PARAMS_TYPE_NAME DelayParamsType
#define MACROS_PATH "Voice/Macros/DelayParamMacros.h"
#include "Voice/Templates/ReflectionCategoryTemplate.h"
#undef CLASS_NAME
#undef PARAMS_TYPE_NAME
#undef MACROS_PATH

	class Reflection
	{
	private:
		static Reflection* s_instance;

	public:
		static const Reflection& Instance();

	private:

#define CATEGORY_INSTANCE(name, categoryName) \
categoryName##Category name \
{ \
	#name, \
	[](const SynthVoiceParams& params) -> const SynthVoiceParams::categoryName##ParamsType& { return params.name##Params; }, \
	[](const SynthVoiceOptionalParams& params) -> const SynthVoiceOptionalParams::categoryName##ParamsType& { return params.name##Params; }, \
	[](const SynthVoiceModulationShortParams& params) -> const SynthVoiceModulationShortParams::categoryName##ParamsType& { return params.name##Params; } \
};
#include "Voice/Macros/SynthVoiceCategoryInstanceMacros.h"
#undef CATEGORY_INSTANCE

		std::vector<ICategory*> m_categories;

		Reflection()
		{
#define CATEGORY_INSTANCE(name, categoryName) \
m_categories.push_back(&name);
#include "Voice/Macros/SynthVoiceCategoryInstanceMacros.h"
#undef CATEGORY_INSTANCE
		}

	public:
#define CATEGORY_INSTANCE(name, categoryName) \
const categoryName##Category& Get##name() const { return name; }
#include "Voice/Macros/SynthVoiceCategoryInstanceMacros.h"
#undef CATEGORY_INSTANCE

		const std::vector<ICategory*> GetCategoriesMap() const { return m_categories; }

		nlohmann::json ToJson(const SynthVoiceParams& params) const
		{
			nlohmann::json json;

			for (ICategory* category : m_categories)
			{
				json[category->GetName()] = category->ToJson(params);
			}

			return json;
		}

		void LoadFromJson(const nlohmann::json& json, SynthVoiceParams& params) const
		{
			for (ICategory* category : m_categories)
			{
				category->LoadFromJson(json[category->GetName()], params);
			}
		}
	};
}

#undef VALUE