DROP TABLE IF EXISTS kv_tag_association CASCADE;
DROP TABLE IF EXISTS api_keys CASCADE;
DROP TABLE IF EXISTS kv_store CASCADE;
DROP TABLE IF EXISTS tags CASCADE;
DROP TABLE IF EXISTS users CASCADE;


CREATE TABLE users (
    -- bigserial 就会自动增加
    -- 
    id BIGSERIAL PRIMARY KEY,
    username VARCHAR(100) NOT NULL UNIQUE,

    -- 哈希加密进来的啊
    password_hash VARCHAR(255) NOT NULL,

    salt VARCHAR(64) NOT NULL,
    
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    last_login_at TIMESTAMPTZ
);


-- CREATE TABLE tags (
--     id BIGSERIAL PRIMARY KEY,
--     user_id BIGINT NOT NULL,
--     name VARCHAR(100) NOT NULL,

--     -- 禁止任何一个用户拥有两个相同的键名
--     UNIQUE (user_id, name),

--     -- 级联
--     FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
-- );


-- CREATE TABLE kv_store (
--     id BIGSERIAL PRIMARY KEY,
--     user_id BIGINT NOT NULL,
--     key VARCHAR(255) NOT NULL,
--     value JSONB NOT NULL,
--     previous_value JSONB NULL, -- Can be NULL if it's the first entry
--     updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),

--     -- A user cannot have duplicate keys.
--     UNIQUE (user_id, key),

--     -- 级联
--     FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
-- );

-- CREATE TABLE api_keys (
--     id BIGSERIAL PRIMARY KEY,
--     user_id BIGINT NOT NULL,
--     key_hash VARCHAR(255) NOT NULL UNIQUE,
--     name VARCHAR(255) NOT NULL,
--     created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),

--     FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
-- );


-- -- 一个 kv 对应一个标签 没有设置就是默认
-- CREATE TABLE kv_tag_association (
--     kv_id BIGINT NOT NULL,
--     tag_id BIGINT NOT NULL,

--     -- The combination of a KV pair and a Tag must be unique.
--     PRIMARY KEY (kv_id, tag_id),

--     -- If the KV pair is deleted, this association is removed.
--     FOREIGN KEY (kv_id) REFERENCES kv_store(id) ON DELETE CASCADE,

--     -- If the Tag is deleted, this association is removed.
--     FOREIGN KEY (tag_id) REFERENCES tags(id) ON DELETE CASCADE
-- );

-- CREATE INDEX IF NOT EXISTS idx_kv_store_key ON kv_store(key);
-- CREATE INDEX IF NOT EXISTS idx_tags_name ON tags(name);

