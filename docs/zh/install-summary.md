# 安装文档摘要 (install.md)

## 概述

llama.cpp 提供了多种预构建版本的安装方式，支持 Windows、Mac 和 Linux 三大平台。

## 支持的安装方式

| 安装方式 | Windows | Mac | Linux |
|---------|---------|-----|-------|
| Winget  | ✅      |     |       |
| Homebrew|         | ✅   | ✅    |
| MacPorts|         | ✅   |       |
| Nix     |         | ✅   | ✅    |

## 各平台安装步骤

### 1. Winget (Windows)

```sh
winget install llama.cpp
```

- 随 llama.cpp 新版本自动更新
- 更多信息: https://github.com/ggml-org/llama.cpp/issues/8188

### 2. Homebrew (Mac 和 Linux)

```sh
brew install llama.cpp
```

- 随 llama.cpp 新版本自动更新
- 更多信息: https://github.com/ggml-org/llama.cpp/discussions/7668

### 3. MacPorts (Mac)

```sh
sudo port install llama.cpp
```

- 参考页面: https://ports.macports.org/port/llama.cpp/details/

### 4. Nix (Mac 和 Linux)

**Flake 启用的安装：**

```sh
nix profile install nixpkgs#llama-cpp
```

**非 Flake 启用的安装：**

```sh
nix-env --file '<nixpkgs>' --install --attr llama-cpp
```

- 自动在 [nixpkgs 仓库](https://github.com/NixOS/nixpkgs/blob/nixos-24.05/pkgs/by-name/ll/llama-cpp/package.nix#L164) 中更新

## 环境配置要点

1. **选择合适的包管理器**：根据操作系统选择对应的包管理器
2. **自动更新**：大部分安装方式都会随新版本自动更新
3. **依赖管理**：包管理器会自动处理依赖关系

## 依赖安装要求

预构建版本已经包含了必要的依赖，无需额外安装。

## 总结

llama.cpp 提供了跨平台的一键安装解决方案，用户可以根据自己的操作系统选择最合适的包管理器进行安装。所有安装方式都支持自动更新，确保用户能够及时获取最新版本。