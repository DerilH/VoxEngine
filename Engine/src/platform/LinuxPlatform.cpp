#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <linux/landlock.h>
#include <linux/prctl.h>
#include <VoxEngine/platform/LinuxPlatform.h>

// Обертки для системных вызовов Landlock через syscall()
#ifndef __NR_landlock_create_ruleset
#define __NR_landlock_create_ruleset 444
#define __NR_landlock_add_rule 445
#define __NR_landlock_restrict_self 446
#endif

static inline int landlock_create_ruleset(const struct landlock_ruleset_attr *attr, size_t size, uint32_t flags) {
    return syscall(__NR_landlock_create_ruleset, attr, size, flags);
}

static inline int landlock_add_rule(int ruleset_fd, enum landlock_rule_type rule_type, const void *rule_attr, uint32_t flags) {
    return syscall(__NR_landlock_add_rule, ruleset_fd, rule_type, rule_attr, flags);
}

static inline int landlock_restrict_self(int ruleset_fd, uint32_t flags) {
    return syscall(__NR_landlock_restrict_self, ruleset_fd, flags);
}

namespace Vox::Platform {

   void LinuxPlatform::enableSandbox(std::filesystem::path allowedDirectory) {
    std::filesystem::path absAllowedDir = std::filesystem::absolute(allowedDirectory);

    struct landlock_ruleset_attr rulesetAttr = {};
    rulesetAttr.handled_access_fs =
        LANDLOCK_ACCESS_FS_EXECUTE |
        LANDLOCK_ACCESS_FS_WRITE_FILE |
        LANDLOCK_ACCESS_FS_READ_FILE |
        LANDLOCK_ACCESS_FS_READ_DIR |
        LANDLOCK_ACCESS_FS_REMOVE_DIR |
        LANDLOCK_ACCESS_FS_REMOVE_FILE |
        LANDLOCK_ACCESS_FS_MAKE_CHAR |
        LANDLOCK_ACCESS_FS_MAKE_DIR |
        LANDLOCK_ACCESS_FS_MAKE_REG |
        LANDLOCK_ACCESS_FS_MAKE_SOCK |
        LANDLOCK_ACCESS_FS_MAKE_FIFO |
        LANDLOCK_ACCESS_FS_MAKE_BLOCK |
        LANDLOCK_ACCESS_FS_MAKE_SYM;

    int rulesetFd = landlock_create_ruleset(&rulesetAttr, sizeof(rulesetAttr), 0);
    if (rulesetFd < 0) {
        std::cerr << "[Platform] Failed to create Landlock ruleset\n";
        return;
    }

    auto allowPath = [&](const std::filesystem::path& path, uint64_t access) {
        if (!std::filesystem::exists(path)) return;

        int fd = open(path.c_str(), O_PATH | O_CLOEXEC);
        if (fd < 0) return;

        struct landlock_path_beneath_attr pathAttr = {};
        pathAttr.allowed_access = access;
        pathAttr.parent_fd = fd;

        landlock_add_rule(rulesetFd, LANDLOCK_RULE_PATH_BENEATH, &pathAttr, 0);
        close(fd);
    };

       uint64_t fullAccess = rulesetAttr.handled_access_fs;
       uint64_t readOnlyAccess = LANDLOCK_ACCESS_FS_READ_FILE |
                                 LANDLOCK_ACCESS_FS_READ_DIR |
                                 LANDLOCK_ACCESS_FS_EXECUTE;

       allowPath(absAllowedDir, fullAccess);

       allowPath("/tmp", fullAccess);

       const char* homeDir = std::getenv("HOME");
       if (homeDir) {
           std::filesystem::path nvCache = std::filesystem::path(homeDir) / ".cache";
           allowPath(nvCache, fullAccess);
       }

       allowPath("/dev", readOnlyAccess | LANDLOCK_ACCESS_FS_WRITE_FILE);
       allowPath("/sys", readOnlyAccess);
       allowPath("/proc", readOnlyAccess);
       allowPath("/usr", readOnlyAccess);
       allowPath("/lib", readOnlyAccess);
       allowPath("/lib64", readOnlyAccess);
       allowPath("/etc", readOnlyAccess);

    if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) < 0) {
        close(rulesetFd);
        return;
    }

    if (landlock_restrict_self(rulesetFd, 0) < 0) {
        std::cerr << "[Platform] Failed to restrict self\n";
    }

       LOG_INFO("Sandbox allowed path {}", absAllowedDir.c_str())
    close(rulesetFd);
}

}