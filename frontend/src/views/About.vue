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
@import "../static/About.css"
</style>
