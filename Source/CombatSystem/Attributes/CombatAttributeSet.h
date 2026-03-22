#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CombatAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class COMBATSYSTEM_API UCombatAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UCombatAttributeSet();

	// Health
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Attributes|Health")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Attributes|Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MaxHealth)

	// Stamina
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina, Category = "Attributes|Stamina")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Stamina)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxStamina, Category = "Attributes|Stamina")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MaxStamina)

	// Combo (useful as an integer-like attribute; stored as float in GAS)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Combo, Category = "Attributes|Combo")
	FGameplayAttributeData Combo;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Combo)

	/** Base damage for attacks (read by abilities / damage execution; set via GE or defaults on pawn). */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Attributes|Combat")
	FGameplayAttributeData BaseDamage;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, BaseDamage)

	// Stun bar (meter). This attribute exists for future stun state/UCues; currently it does not affect damage.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stun, Category = "Attributes|Stun")
	FGameplayAttributeData Stun;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Stun)

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Clamp Health / Stun after GE modifies them.
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

protected:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldStamina) const;
	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina) const;
	UFUNCTION()
	void OnRep_Combo(const FGameplayAttributeData& OldCombo) const;

	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldBaseDamage) const;

	UFUNCTION()
	void OnRep_Stun(const FGameplayAttributeData& OldStun) const;
};

