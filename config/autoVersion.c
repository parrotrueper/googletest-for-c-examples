/* clang-format off */
/**
 * \file autoVersion.c
 * \brief example firmware version, do not edit.
 * Automatically generated do not edit.
 * see generate-version-files.sh for details
 */
#include "autoVersion.h"

/**
 * \brief Get the version
 * \return version structure pointer
 */
const st_auto_version_t* autoVersion_getInfo(void)
{
	static const st_auto_version_t f_stVersion = {
		.pVersion       = "0.0.1",
		.pCommitHash    = "2e2a7a0",
		.pBranch        = "main",
		.pChanges       = "DIRTY",
	};
    return &f_stVersion;
}
/* clang-format on */
