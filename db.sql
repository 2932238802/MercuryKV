DROP TABLE IF EXISTS kv_tag_association CASCADE;
DROP TABLE IF EXISTS api_keys CASCADE;
DROP TABLE IF EXISTS kv_store CASCADE;
DROP TABLE IF EXISTS tags CASCADE;
DROP TABLE IF EXISTS users CASCADE;


CREATE TABLE users (
    user_id BIGSERIAL PRIMARY KEY,

    --------------------------------------------------------
    username VARCHAR(100) NOT NULL UNIQUE,
    email VARCHAR(100) NOT NULL UNIQUE,
    password_hash VARCHAR(255) NOT NULL,
    salt VARCHAR(64) NOT NULL,
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    last_login_at TIMESTAMPTZ
    --------------------------------------------------------

);

-- 这个是 标签名字 对应用户 就是用户创建的标签
CREATE TABLE tags (
    tag_id BIGSERIAL PRIMARY KEY,

    --------------------------------------------------------
    user_id BIGINT NOT NULL,
    tag_name VARCHAR(100) NOT NULL,
    --------------------------------------------------------

    UNIQUE (user_id, tag_name),
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE
);

-- kv 表
CREATE TABLE kv_store (
    kv_id BIGSERIAL PRIMARY KEY,

    --------------------------------------------------------
    user_id BIGINT NOT NULL,
    key_input VARCHAR(255) NOT NULL,
    value_input JSONB NOT NULL,
    previous_value JSONB NULL, 
    updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    --------------------------------------------------------

    UNIQUE (user_id, key_input),
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE
);

-- tag 和 kv的表
CREATE TABLE kv_tag_association (

    --------------------------------------------------------
    kv_id BIGINT NOT NULL,
    tag_id BIGINT NOT NULL,
    --------------------------------------------------------

    PRIMARY KEY (kv_id, tag_id),
    FOREIGN KEY (kv_id) REFERENCES kv_store(kv_id) ON DELETE CASCADE,
    FOREIGN KEY (tag_id) REFERENCES tags(tag_id) ON DELETE CASCADE
);

-- 一个用户的 一个api 接口
CREATE TABLE api_keys (
    api_id BIGSERIAL PRIMARY KEY,

    --------------------------------------------------------
    user_id BIGINT NOT NULL,
    key_hash VARCHAR(255) NOT NULL UNIQUE,
    api_name VARCHAR(255) NOT NULL,
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    --------------------------------------------------------

    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE
);