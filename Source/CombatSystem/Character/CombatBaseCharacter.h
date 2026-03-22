// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

class AActor;
class UCombatAttributeSet;
class UGameplayEffect;
#include "CombatBaseCharacter.generated.h"

UCLASS()
class COMBATSYSTEM_API ACombatBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACombatBaseCharacter();

	/** Simple patrol points for AI enemies – assigned per-instance in the level */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "AI|Patrol")
	TArray<AActor*> PatrolPoints;

	/** Time to wait at each patrol point (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Patrol")
	float PatrolWaitTime = 1.0f;

	/** Current patrol index (for AI controllers / BT tasks to read if needed) */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AI|Patrol")
	int32 CurrentPatrolIndex = 0;

	//Ability System
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	class UAbilitySystemComponent* AbilitySystemComponent;

	// Attribute Set (Health / Stamina / Combo)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	TObjectPtr<UCombatAttributeSet> CombatAttributes;

	/** GE class used to apply damage (Health) via ExecutionCalculation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Damage")
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Returns the current patrol point actor, or nullptr if invalid / none */
	UFUNCTION(BlueprintCallable, Category = "AI|Patrol")
	AActor* GetCurrentPatrolPoint() const;

	/**
	 * Advance to the next patrol point and return it.
	 * If there are no valid patrol points, returns nullptr.
	 */
	UFUNCTION(BlueprintCallable, Category = "AI|Patrol")
	AActor* GetNextPatrolPoint();

	/** Hook for Blueprint/AI to react when a player is detected in this character's detection area */
	UFUNCTION(BlueprintImplementableEvent, Category = "AI|Detection")
	void OnPlayerDetected(AActor* DetectedActor);

	/**
	 * Call this from Blueprint (e.g., overlap) to trigger `OnPlayerDetected`.
	 * The event itself is implemented in Blueprint, but calling events directly from Blueprint can be awkward.
	 */
	UFUNCTION(BlueprintCallable, Category = "AI|Detection")
	void NotifyPlayerDetected(AActor* DetectedActor);

	/**
	 * Apply GAS damage to a target. Base damage is taken from this character's CombatAttributes->BaseDamage.
	 * DamageScale (optional) is passed via SetByCaller Data.Damage.Scale (1.0 = no per-hit extra multiply).
	 */
	UFUNCTION(BlueprintCallable, Category = "GAS|Damage")
	void ApplyGE_Damage(AActor* TargetActor, float DamageScale = 1.0f);
};
