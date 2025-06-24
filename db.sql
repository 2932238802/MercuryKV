
DROP DATABASE IF EXISTS Mercury;
CREATE DATABASE Mercury CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
-- 支持一下中文和表情包
USE Mercury;

CREATE TABLE users (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(255) NOT NULL UNIQUE COMMENT '用户名，唯一',

    email VARCHAR(255) NOT NULL UNIQUE COMMENT '邮箱，唯一',

    hashed_password VARCHAR(255) NOT NULL COMMENT '安全哈希后的密码',

    created_at DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',

    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '最后更新时间'
) ENGINE=InnoDB COMMENT='用户信息表';


CREATE TABLE user_kv_store (
    user_id BIGINT NOT NULL COMMENT '外键,关联到users.id',

    item_key VARCHAR(255) NOT NULL COMMENT '用户定义的键',

    item_value BLOB COMMENT '存储的值，可以是任意二进制数据',

    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,

    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,

    PRIMARY KEY (user_id, item_key),

    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
) ENGINE=InnoDB COMMENT='用户键值存储表';

CREATE TABLE tags (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,

    user_id BIGINT NOT NULL COMMENT '外键,关联到users.id',

    name VARCHAR(255) NOT NULL COMMENT '标签名称',

    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,

    UNIQUE (user_id, name),
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
) ENGINE=InnoDB COMMENT='用户自定义标签表';

CREATE TABLE item_tags (
    user_id BIGINT NOT NULL,

    item_key VARCHAR(255) NOT NULL,

    tag_id BIGINT NOT NULL,
    
    PRIMARY KEY (user_id, item_key, tag_id),

    FOREIGN KEY (user_id, item_key) REFERENCES user_kv_store(user_id,item_key) ON DELETE CASCADE,

    FOREIGN KEY (tag_id) REFERENCES tags(id) ON DELETE CASCADE
) ENGINE=InnoDB COMMENT='键值项与标签的关联表';



