# Gemini 相关配置说明

本目录包含 Claude Code Agent 使用的配置文件。

## 文件说明

### settings.json

Claude Code Agent 的会话配置文件，用于存储 Agent 相关的设置和偏好。

配置项说明：
- `contextFileName`: 指定上下文文件名（如 AGENTS.md）

## 使用说明

此目录和配置文件主要由 Claude Code Agent 自动管理，一般情况下无需手动修改。

如需调整 Agent 行为，建议通过 `.claude/settings.json` 进行全局配置。

## 相关文档

- [AGENTS.md](../AGENTS.md) - Agent 工作流程和规范
- [CLAUDE.md](../CLAUDE.md) - 项目说明和使用指南