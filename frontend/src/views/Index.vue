<script setup>
import { useRouter } from 'vue-router';
import service from '../components/request';
import { ShowCustomModal } from '../components/show';

const router = useRouter();
const API_PATH = {
    TOKEN_CHECK: "/Login/checktoken"
}

const CheckTokenForLastLogin = async () => {
    try {
        const response = await service.get(API_PATH.TOKEN_CHECK);
        return response;
    } catch (error) {
        console.error("Token 验证失败!", error.message);
        return null;
    }
}

const Login = async () => {
    const responseData = await CheckTokenForLastLogin();
    if (!responseData || responseData.code !== 200) {
        router.push({ name: "RegisterAndLogin" });
    } else {
        router.push({ name: "Mercury" });
        const username = localStorage.getItem("UserName");
        ShowCustomModal(`欢迎回来 ${username}`);
    }
}

const About = () => {
    router.push({ name: "About" });
    return;
}
</script>

<template>
    <div class="page-container">

        <div class="panel left" @click="Login">
            <div class="content">
                <h2>登 录</h2>
                <p>LOGIN</p>
            </div>
        </div>

        <div class="panel right" @click="About">
            <div class="content">
                <h2>关 于</h2>
                <p>ABOUT</p>
            </div>
        </div>
    </div>
</template>

<style scoped>
@import url('https://fonts.googleapis.com/css2?family=Poppins:wght@300;700&display=swap');

.page-container {
    display: flex;
    width: 100vw;
    height: 100vh;
    margin: 0;
    padding: 0;
    overflow: hidden;
    background-color: #1a1a1a;
    font-family: 'Poppins', 'Noto Sans SC', sans-serif;
}

.panel {
    flex: 1;
    position: relative;
    display: flex;
    justify-content: center;
    align-items: center;
    cursor: pointer;
    overflow: hidden;
    transition: all 0.6s ease-out;
}

.left {
    background-color: #111;
}

.right {
    background-color: #f0f0f0;
}

.content {
    position: relative;
    z-index: 2;
    text-align: center;
    /* 优化点 1: 动画曲线同步调整 */
    transition: color 0.6s ease-out;
}

.left .content {
    color: #fff;
}

.right .content {
    color: #111;
}

.content h2 {
    font-size: 3.5vw;
    font-weight: 700;
    text-transform: uppercase;
    letter-spacing: 0.2em;
    margin: 0;
}

.content p {
    font-size: 0.9vw;
    font-weight: 300;
    letter-spacing: 0.1em;
    text-transform: uppercase;
    opacity: 0.7;
}

.panel::before {
    content: '';
    position: absolute;
    top: 0;
    width: 100%;
    height: 100%;
    z-index: 1;
    transition: transform 0.6s ease-out;
}

.left::before {
    background-color: #fff;
    transform: translateX(-100%);
}

.right::before {
    background-color: #111;
    transform: translateX(100%);
}

.panel:hover::before {
    transform: translateX(0);
}

.panel:hover .content {
    color: #111;
}

.right:hover .content {
    color: #fff;
}

.page-container:hover .panel:not(:hover) {
    transform: scale(0.98);
    filter: blur(1px) opacity(0.9);
}


@media (max-width: 768px) {
    .page-container {
        flex-direction: column;
    }

    .content h2 {
        font-size: 8vw;
    }

    .content p {
        font-size: 2.5vw;
    }
}
</style>
