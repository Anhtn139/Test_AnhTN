#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "CombatDamageExecutionCalculation.generated.h"

/**
 * GE execution for dealing damage (Health).
 *
 * Base damage comes from the source actor's UCombatAttributeSet::BaseDamage (not from BP input).
 *
 * Optional SetByCaller on the GameplayEffect Spec:
 *  - Data.Damage.Scale (float)       : per-hit multiplier (default 1.0)
 */
UCLASS()
class COMBATSYSTEM_API UCombatDamageExecutionCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UCombatDamageExecutionCalculation();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                   FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

private:
	/** How much combo contributes to damage. FinalMultiplier = 1 + Combo * ComboDamagePerPoint */
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float ComboDamagePerPoint = 0.15f;
};

