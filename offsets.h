#ifndef ATLAS_GENERATOR_OFFSETS_H
#define ATLAS_GENERATOR_OFFSETS_H

/*
 * Represents characters in SpeedRunners.
 */
enum character
{
	BeachBodySpeedRunner,
	Buckshot,
	Burger,
	CannonFyodor,
	CinnamonToastKen,
	Cosmonaut,
	Dashwell,
	DiamondDiva,
	DoctorSmart,
	Dodger,
	Excel,
	Falcon,
	Flamenco,
	FortKnight,
	GangBeast,
	Gil,
	Hothead,
	Jailbird,
	JesseCox,
	Loading,
	Lucjadore,
	ManekiNeko,
	Markiplier,
	Moonraker,
	MrQuick,
	Octodad,
	Payday,
	PeanutButterGamer,
	PewDiePie,
	Salem,
	Scout,
	SherlockBones,
	Showtime,
	SkullDuggery,
	Speedrunner,
	Strippin,
	SummertimeCosmonaut,
	UberHaxorNova,
	Unic,
	VelociTrex,
	XL
};

/*
 * Get the x offset for a character
 */
int character_offset_x(int character);

/*
 * Check for a specific file extension.
 */
int character_offset_y(int character);

#endif //ATLAS_GENERATOR_OFFSETS_H
