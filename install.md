# 项目构建

## 需求分析

项目背景与目标
用户角色分析
功能需求列表
非功能需求

- 绝对不能明文存储。它必须经过哈希加密处理后 （账号密码存储 ！）
- 如果用户在K-V中存储其他密码或敏感信息，我们后端不应该也无法知道其内容。在**前端（Vue）**进行加密，将密文作为Value存到后端。后端只负责忠实地保管这串加密后的数据
- 需要一个API来列出某个用户的所有K-V对
- 修改密码是一个独立、高权限的功能 （比如发送验证码）
- 可以选择项目标签 把自己的kv 全部拿出来

## 项目目录
前后端分离

├── backend/                      # 存放 C++ 后端代码 (就是之前的 NexusCore)
│   ├── CMakeLists.txt
│   └── src/
│       ├── api/
│       ├── services/
│       ├── db/
│       └── main.cpp
├── frontend/                     # 存放 Vue.js 前端代码
│   ├── public/
│   ├── src/
│   │   ├── components/           # Vue 组件
│   │   │   └── UserComponent.vue
│   │   ├── services/             # 存放 gRPC 客户端相关代码
│   │   │   └── grpc-client.ts
│   │   ├── App.vue               # 主组件
│   │   └── main.ts               # Vue 入口文件
│   ├── package.json
│   └── vite.config.ts
├── proxy/                        # 存放 Envoy 代理的配置
│   └── envoy.yaml
└── proto/                        # 共享的 .proto 文件 (提升到顶层)
    └── user_service.proto

## 数据库设计

### 
- 四个表格
- users
- user_kv_store
- tags
- item_tags

### 查询语句

查询用户是不是存在
SELECT id FROM users WHERE username = ? OR email = ?;

插入用户
INSERT INTO users (username, email, hashed_password) VALUES (?, ?, ?);

验证用户信息 查询用户账号密码 是不是正确
SELECT id, hashed_password FROM users WHERE username = ?;

插入kv
INSERT INTO user_kv_store (user_id,key, item_value) VALUES (?, ?, ?)
ON DUPLICATE KEY UPDATE item_value = VALUES(item_value), updated_at = CURRENT_TIMESTAMP;

删除标签
DELETE FROM item_tags WHERE user_id = ? AND item_key = ?;

获取 kv值
SELECT item_value, created_at, updated_at FROM user_kv_store WHERE user_id = ? AND _key_ = ?;

获取
SELECT k._key_, k.created_at, k.updated_at
FROM user_kv_store k
JOIN item_tags it ON k.user_id = it.user_id AND k._key_ = it.item_key
JOIN tags t ON it.tag_id = t.id
WHERE k.user_id = ? AND t.name = ?
ORDER BY k.updated_at DESC;

获取所有标签
SELECT name FROM tags WHERE user_id = ? ORDER BY name ASC;

## 文档图

- ER图
- 关系模式
- 流程图
