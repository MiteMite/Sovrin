// 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EquipmentInterface.generated.h"

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	Weapon      UMETA(DisplayName = "Weapon"),
	Armor       UMETA(DisplayName = "Armor"),
	Accessory   UMETA(DisplayName = "Accessory"),
	Consumable  UMETA(DisplayName = "Consumable"),
	Tool        UMETA(DisplayName = "Tool")
};

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	None            UMETA(DisplayName = "None"),
	MainHand        UMETA(DisplayName = "Main Hand"),
	OffHand         UMETA(DisplayName = "Off Hand"),
	Head            UMETA(DisplayName = "Head"),
	Chest           UMETA(DisplayName = "Chest"),
	Legs            UMETA(DisplayName = "Legs"),
	Feet            UMETA(DisplayName = "Feet"),
};

UINTERFACE(MinimalAPI, BlueprintType)
class UEquipmentInterface : public UInterface
{
	GENERATED_BODY()
};

class SOVRIN_API IEquipmentInterface
{
	GENERATED_BODY()

public:
	// Basic equipment information
	virtual FString GetEquipmentName() const = 0;
	virtual FString GetDescription() const = 0;
	virtual EEquipmentType GetEquipmentType() const = 0;
	virtual EEquipmentSlot GetEquipmentSlot() const = 0;
	virtual int32 GetItemLevel() const = 0;
	virtual float GetWeight() const = 0;
	virtual int32 GetValue() const = 0;
    
	// Equipment actions
	virtual bool CanEquip(AActor* Character) const = 0;
	virtual bool Equip(AActor* Character) = 0;
	virtual bool Unequip(AActor* Character) = 0;
	virtual void OnEquipped(AActor* Character) = 0;
	virtual void OnUnequipped(AActor* Character) = 0;
};
