<script setup>
import { useRouter } from 'vue-router';
import service from '@/utils/request';
import { ShowCustomModal } from '@/utils/show';
import { TOKEN_CHECK } from '@/utils/api';

const router = useRouter();

const Login = async () => {
    try {
        const responsedata = await TOKEN_CHECK();
        console.log("验证成功，后端返回:", responsedata);
        console.log("后端返回的 code:", responsedata.code);
        if (responsedata.code === 200) { 
            router.push({ name: "Mercury" });
            const username = localStorage.getItem("UserName");
            ShowCustomModal(`欢迎回来 ${username}`);
        } else {
            ShowCustomModal(responsedata.message || "验证失败");
            router.push({ name: "RegisterAndLogin" });
        }
    } catch (error) {
        console.error("登录失败:", error);
        router.push({ name: "RegisterAndLogin" });
    }
};


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
@import "@/static/Index.css"

</style>
