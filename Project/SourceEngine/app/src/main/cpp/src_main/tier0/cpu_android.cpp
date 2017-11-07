//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//========= Written by Triang3l for D0G. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include <stdio.h>
#include <tier0/platform.h>

uint64 GetCPUFreqFromDevices()
{
	FILE *fp;
	char line[16];
	unsigned int khz = 0;

	if ((fp = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", "r")) == NULL)
		return 0;

	if (fgets(line, sizeof(line), fp) != NULL)
		sscanf(line, "%u", &khz);

	fclose(fp);

	return (uint64)khz * 1000;
}

unsigned int GetCPUCountFromPROC()
{
	FILE *fp;
	char line[1024];
	unsigned int count = 0;

	if ((fp = fopen("/proc/cpuinfo", "r")) == NULL)
		return 1;

	while (fgets(line, sizeof(line), fp) != NULL)
	{
		if (strncmp(line, "processor\t:", sizeof("processor\t:") - 1) == 0)
			++count;
	}

	fclose(fp);

	if (count == 0)
		count = 1;
	return count;
}

bool CheckNEONTechnologyFromPROC()
{
#ifndef __arm__
	return false;
#else
	FILE *fp;
	char line[1024];
	bool hasNEONFeature = false, isARMv7 = false;

	if ((fp = fopen("/proc/cpuinfo", "r")) == NULL)
		return true; // ARMv7 CPUs without NEON are very rare (Tegra 2 only), so assume NEON support even if failed to open.

	while (fgets(line, sizeof(line), fp) != NULL)
	{
		if (strncmp(line, "Features\t:", sizeof("Features\t:") - 1) == 0)
		{
			if (strstr(line + sizeof("Features\t:") - 1, " neon ") != NULL)
				hasNEONFeature = true;
		}
		else if (strcmp(line, "CPU architecture: 7\n") == 0)
		{
			isARMv7 = true;
		}
	}

	return hasNEONFeature || !isARMv7; // If not ARMv7, it's AArch64 or a newer architecture (6 is not supported).
#endif
}
