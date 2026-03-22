// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatBaseCharacter.h"
#include "Attributes/CombatAttributeSet.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "GameplayTagsManager.h"

// Sets default values
ACombatBaseCharacter::ACombatBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//Add the ability system component
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	CombatAttributes = CreateDefaultSubobject<UCombatAttributeSet>(TEXT("CombatAttributes"));
	if (AbilitySystemComponent && CombatAttributes)
	{
		AbilitySystemComponent->AddSpawnedAttribute(CombatAttributes);
	}
}

// Called when the game starts or when spawned
void ACombatBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACombatBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACombatBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* ACombatBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACombatBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Initialize the ability system component for the new controller
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

AActor* ACombatBaseCharacter::GetCurrentPatrolPoint() const
{
	if (PatrolPoints.IsValidIndex(CurrentPatrolIndex))
	{
		return PatrolPoints[CurrentPatrolIndex];
	}

	return nullptr;
}

AActor* ACombatBaseCharacter::GetNextPatrolPoint()
{
	if (PatrolPoints.Num() == 0)
	{
		return nullptr;
	}

	// Advance index in a simple loop
	CurrentPatrolIndex = (CurrentPatrolIndex + 1) % PatrolPoints.Num();

	if (!PatrolPoints.IsValidIndex(CurrentPatrolIndex))
	{
		return nullptr;
	}

	return PatrolPoints[CurrentPatrolIndex];
}

void ACombatBaseCharacter::NotifyPlayerDetected(AActor* DetectedActor)
{
	if (!DetectedActor)
	{
		return;
	}

	OnPlayerDetected(DetectedActor);
}

void ACombatBaseCharacter::ApplyGE_Damage(AActor* TargetActor, float DamageScale)
{
	if (!TargetActor || !AbilitySystemComponent || !DamageGameplayEffectClass)
	{
		return;
	}

	IAbilitySystemInterface* TargetASCInterface = Cast<IAbilitySystemInterface>(TargetActor);
	if (!TargetASCInterface)
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = TargetASCInterface->GetAbilitySystemComponent();
	if (!TargetASC)
	{
		return;
	}

	// Build effect spec
	const FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DamageGameplayEffectClass, 1.0f, EffectContext);
	if (!SpecHandle.IsValid())
	{
		return;
	}

	// Scale only; base damage comes from source BaseDamage attribute in ExecutionCalculation
	const FGameplayTag DamageScaleTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Data.Damage.Scale"), false);
	if (DamageScaleTag.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(DamageScaleTag, DamageScale);
	}
	AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
}
