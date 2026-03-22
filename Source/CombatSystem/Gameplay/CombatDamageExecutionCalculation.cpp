#include "Gameplay/CombatDamageExecutionCalculation.h"

#include "Attributes/CombatAttributeSet.h"

#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagsManager.h"

UCombatDamageExecutionCalculation::UCombatDamageExecutionCalculation()
{
}

// Tags used for SetByCaller values on the GameplayEffect spec.
static FGameplayTag GetTagOrEmpty(const TCHAR* TagName)
{
	if (!TagName)
	{
		return FGameplayTag();
	}

	// ErrorIfNotFound = false -> return invalid tag if not registered.
	return UGameplayTagsManager::Get().RequestGameplayTag(FName(TagName), false);
}

void UCombatDamageExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                               FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	if (!TargetASC || !SourceASC)
	{
		return;
	}

	const FGameplayTag DamageScaleTag = GetTagOrEmpty(TEXT("Data.Damage.Scale"));
	const float DamageScale = DamageScaleTag.IsValid() ? Spec.GetSetByCallerMagnitude(DamageScaleTag, false, 1.0f) : 1.0f;

	const UCombatAttributeSet* SourceAttr = SourceASC->GetSet<UCombatAttributeSet>();
	if (!SourceAttr)
	{
		return;
	}

	const float BaseDamageValue = SourceAttr->GetBaseDamage();
	float Combo = SourceAttr->GetCombo();

	const float ComboMultiplier = 1.0f + (Combo * ComboDamagePerPoint);
	const float FinalDamage = FMath::Max(0.0f, BaseDamageValue * DamageScale * ComboMultiplier);

	// Apply health damage: subtract FinalDamage from Health attribute.
	if (FinalDamage > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(UCombatAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, -FinalDamage));
	}

}

