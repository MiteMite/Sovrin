// 

#pragma once

#include "CoreMinimal.h"
#include "EquipmentInterface.h"
#include "Pickupable.h"
#include "Weapon.h"
#include "ProjectileLauncher.generated.h"

UCLASS()
class SOVRIN_API AProjectileLauncher : public AWeapon, public IEquipmentInterface, public IPickupable
{
	GENERATED_BODY()

public:
	AProjectileLauncher();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FString ItemName = "Base Equipment";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FString Description = "A basic piece of equipment";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	EEquipmentType EquipmentType = EEquipmentType::Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	EEquipmentSlot EquipmentSlot = EEquipmentSlot::MainHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	int32 ItemLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	float Weight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	int32 Value = 100;

public:
	virtual FString GetEquipmentName() const override { return ItemName; }
	virtual FString GetDescription() const override { return Description; }
	virtual EEquipmentType GetEquipmentType() const override { return EquipmentType; }
	virtual EEquipmentSlot GetEquipmentSlot() const override { return EquipmentSlot; }
	virtual int32 GetItemLevel() const override { return ItemLevel; }
	virtual float GetWeight() const override { return Weight; }
	virtual int32 GetValue() const override { return Value; }

	virtual bool CanEquip(AActor* Character) const override;
	virtual bool Equip(AActor* Character) override;
	virtual bool Unequip(AActor* Character) override;
	virtual void OnEquipped(AActor* Character) override;
	virtual void OnUnequipped(AActor* Character) override;
	
	virtual bool Pickup() override;
};
