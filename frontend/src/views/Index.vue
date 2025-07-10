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
@import "../static/Index.css"

</style>
