// 


#include "Sovrin/Public/ProjectileLauncher.h"



AProjectileLauncher::AProjectileLauncher()
{

}

bool AProjectileLauncher::CanEquip(AActor* Character) const
{
	return true;
}

bool AProjectileLauncher::Equip(AActor* Character)
{
	return true;
}

bool AProjectileLauncher::Unequip(AActor* Character)
{
	return true;
}

bool AProjectileLauncher::Pickup()
{
	return true;
}

void AProjectileLauncher::OnEquipped(AActor* Character)
{
	
}

void AProjectileLauncher::OnUnequipped(AActor* Character)
{
	
}