<script setup>
const TeamMembers = [
    { name: 'LosAngelous', role: '全栈开发者', avatar: '' },
]
</script>

<template>
    <div class="about-page-container">

        <router-link :to="{ name: 'Index' }" class="back-link">
            ← 返回首页
        </router-link>

        <h1>Mercury</h1>
        <hr class="title-hr">

        <section class="about-section">
            <h2>我们的使命</h2>
        </section>

        <section class="about-section">
            <h2>技术栈与理念</h2>
            <ul>
                <li><strong>前端：</strong> Vue 3 (Composition API) + Vite + Pinia + Vue Router</li>
                <li><strong>后端：</strong> C++ Drogon 框架，追求极致性能与高并发。</li>
                <li><strong>数据库：</strong> PostgreSQL</li>
                <li><strong>设计理念：</strong> 我们遵循极简主义设计...</li>
            </ul>
        </section>

        <section class="about-section">
            <h2>队伍:</h2>
            <div class="team-container">
                <div v-for="member in TeamMembers" :key="member.name" class="team-member">
                    <div class="avatar-placeholder"></div>
                    <h3>{{ member.name }}</h3>
                    <p class="role">{{ member.role }}</p>
                </div>
            </div>
        </section>

        <section class="about-section">
            <h2>联系我们</h2>
            <p>
                我们非常欢迎任何形式的反馈、建议或合作。你可以通过以下方式找到我们：
            </p>
            <p>
                <strong>电子邮箱：</strong> <a href="mailto:19857198709@163.com">19857198709@163.com</a>
            </p>
            <p>
                <strong>GitHub：</strong> <a href="https://github.com/2932238802/MercuryKV.git"
                    target="_blank">https://github.com/2932238802/MercuryKV.git</a>
            </p>
        </section>

    </div>
</template>

<style scoped>
.about-page-container {
    /* 当浏览器窗口宽度大于 800px 时，容器宽度固定为 800px
    当窗口宽度小于 800px 时，容器会等比例缩小，保证响应式布局 */
    /* max-width: 800px; */

    /* 先上下 再左右 */
    margin: 0 auto;

    /* 内边距为 40px */
    padding: 30px 300px;

    /*  */
    background-color: #121212;

    /*  */
    color: #e0e0e0;

    /*  */
    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
    line-height: 1.7;
}

/* 返回 链接的颜色 配置 */
.back-link {

    display: inline-block;
    margin-bottom: 40px;
    padding: 8px 15px;
    border: 1px solid #555;
    border-radius: 4px;
    color: #e0e0e0;
    text-decoration: none;
    font-size: 0.9rem;
    transition: background-color 0.2s, border-color 0.2s;
}

.back-link:hover {
    background-color: #2a2a2a;
    border-color: #777;
}

h1 {
    font-family: Georgia, 'Times New Roman', Times, serif;
    font-size: 2.5rem;
    color: #ffffff;
    border-bottom: 1px solid #444;
    padding-bottom: 10px;
    margin-bottom: 0;
}

.title-hr {
    display: none;
}

h2 {
    font-family: Georgia, 'Times New Roman', Times, serif;
    font-size: 1.8rem;
    color: #ffffff;
    margin-top: 40px;
    margin-bottom: 15px;
    border-bottom: 1px solid #444;
    padding-bottom: 5px;
}

h3 {
    font-family: Georgia, 'Times New Roman', Times, serif;
    font-size: 1.2rem;
    color: #f5f5f5;
    margin-top: 10px;
    margin-bottom: 5px;
}

p {
    font-size: 1rem;
    margin-bottom: 1em;
}

a {
    color: #64b5f6;
    text-decoration: underline;
    transition: color 0.2s;
}

a:hover {
    color: #90caf9;
    text-decoration: none;
}

a.back-link {
    color: #e0e0e0;
    text-decoration: none;
}

a.back-link:hover {
    color: #e0e0e0;
}

ul {
    padding-left: 20px;
}

li {
    margin-bottom: 10px;
}

.team-container {
    display: flex;
    flex-wrap: wrap;
    gap: 30px;
    justify-content: center;
    text-align: center;
}

.team-member {
    flex-basis: 200px;
}

.avatar-placeholder {
    width: 120px;
    height: 120px;
    background-color: #333;
    border-radius: 50%;
    margin: 0 auto;
    border: 1px solid #444;
}

.role {
    font-size: 0.9rem;
    color: #aaaaaa;
}
</style>
