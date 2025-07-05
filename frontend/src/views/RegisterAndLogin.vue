<script setup>
import { ref } from 'vue';
import service from '../components/request';
import { useRouter } from 'vue-router';
import { ShowCustomModal } from '../components/show'

// ------------------------------
// 绑定页面的输入内容
const user_name_register = ref("");
const password_register = ref("");
const username_login = ref("");
const password_login = ref("");
const email_register = ref("");
const show_login_warning_for_user_name = ref(false)
const show_login_warning_for_password = ref(false)
const show_register_warning_for_user_name = ref(false)
const show_register_warning_for_password = ref(false)
const show_register_warning_for_email = ref(false)
const API_PATHS = {
    LOGIN: '/Login/login',
    REGISTER: '/Register/register'
};
const router = useRouter();

// ------------------------------
// 登录和注册的切换
const isrightpanelactive = ref(false);
const ShowSignIn = () => {
    show_register_warning_for_user_name.value = false
    show_register_warning_for_password.value = false
    show_register_warning_for_email.value = false
    isrightpanelactive.value = false;
}
const ShowSignUp = () => {
    show_login_warning_for_user_name.value = false
    show_login_warning_for_password.value = false
    isrightpanelactive.value = true;
}
const ReturnIndex = () => {
    router.push({ name: "Index" });
}


// ------------------------------
// 向后端发送请求
// 登录
// 注册成功 201
// 然后保存token 到本地
// TODO: 后端看一眼 200
// console.log("登录成功!");
// token本地保存一下
const Login = async () => {

    // 警告要重置一下
    show_login_warning_for_user_name.value = false;
    show_login_warning_for_password.value = false;

    if (!username_login.value) {
        show_login_warning_for_user_name.value = true;
        return;
    }

    if (!password_login.value) {
        show_login_warning_for_password.value = true;
        return;
    }

    console.log("即将发起登录请求...");
    // 理解一下会话： 就是手环 编号
    // 理解一下 token : 
    try {
        const login_info = {
            username: username_login.value,
            password: password_login.value
        }

        const response = await service.post(
            API_PATHS.LOGIN,
            login_info
        )

        alert("登录成功，后端返回的完整数据是:", JSON.stringify(response, null, 2));
        localStorage.setItem('AuthToken', response.token);
        localStorage.setItem('UserName',response.username);
        localStorage.setItem('UserId', response.user_id);

        router.push({ name: "Mercury" })
        ShowCustomModal(response.message);
    }
    catch (error) {
        // 日志输出一下
        console.error("登录失败:", error.message);
        ShowCustomModal(`登录失败：${error.message}`);
    }
}

// ------------------------------
// 向后端发送请求
// 注册
const Register = async () => {
    // 初始化赋值
    show_register_warning_for_user_name.value = false;
    show_register_warning_for_password.value = false;
    show_register_warning_for_email.value = false;

     if (!user_name_register.value) {
        show_register_warning_for_user_name.value = true;
        return; 
    }
    if (!password_register.value) {
        show_register_warning_for_password.value = true;
        return; 
    }
    if (!email_register.value) {
        show_register_warning_for_email.value = true
        return; 
    }

    try {
        // 注册信息
        const register_info = {
            username: user_name_register.value,
            password: password_register.value,
            email: email_register.value
        }
        const response = await service.post(
            API_PATHS.REGISTER,
            register_info
        )
        const token = response.token;
        const user_id = response.user_id;
        const username = response.username;

        localStorage.setItem('AuthToken', token);
        localStorage.setItem('UserId', user_id);
        localStorage.setItem('UserName',username);
        router.push({ name: "Mercury" });
        ShowCustomModal(response.message);                        
    }
    catch (error) {
        console.error("注册失败:", error.message);
        ShowCustomModal(`注册失败：${error.message}`);
    }
}

</script>

<template>
    <div class="page-body">
        <div class="container" :class="{ 'right-panel-active': isrightpanelactive }">

            <div class="form-container sign-up-container">
                <div class="return-button" @click="ReturnIndex"> Return </div>
                <form @submit.prevent>
                    <h1>创建账户</h1>
                    <input type="text" placeholder="用户名" v-model="user_name_register" minlength="8" maxlength="14"
                        required />
                    <input type="email" placeholder="邮箱" v-model="email_register" required />
                    <input type="password" placeholder="密码" v-model="password_register" minlength="8" maxlength="20"
                        pattern="^(?=.*[A-Za-z])(?=.*\d)[A-Za-z\d]{8,20}$" required />
                    <!-- 
                    
                    (1) : 是 v-bind: 的简写 
                    (2) : 键值对 'right-panel-active': isrightpanelactive
                        (1) isrightpanelactive 这个必须是布尔类型
                    
                    (1) : v-if 是一个条件渲染指令。它会根据表达式的真假值，来决定是否渲染这个元素
                    如果 v-if 的值为 true，元素会被创建并插入到 DOM 中
                    如果 v-if 的值为 false，元素会从 DOM 中被移除
                    
                    -->

                    <div v-if="show_register_warning_for_user_name" class="warning_word">用户名输入格式错误!</div>
                    <div v-if="show_register_warning_for_password" class="warning_word">密码输入格式错误!</div>
                    <div v-if="show_register_warning_for_email" class="warning_word">邮箱输入格式错误!</div>

                    <button @click="Register">注 册</button>
                </form>
            </div>

            <div class="form-container sign-in-container">
                <div class="return-button" @click="ReturnIndex"> Return </div>
                <form @submit.prevent>
                    <h1>登 录</h1>
                    <input type="text" placeholder="用户名" v-model="username_login" />
                    <input type="password" placeholder="密码" v-model="password_login" />
                    <div v-if="show_login_warning_for_user_name" class="warning_word">用户名输入格式错误!</div>
                    <div v-if="show_login_warning_for_password" class="warning_word">密码输入格式错误!</div>

                    <button @click="Login">登 录</button>
                </form>
            </div>

            <div class="overlay-container">

                <div class="overlay">
                    <div class="overlay-panel overlay-left">
                        <h1>欢迎回来!</h1>
                        <p>为了与我们保持联系</p>
                        <p>请使用您的个人信息登录</p>
                        <button class="ghost" @click="ShowSignIn">去登录</button>
                    </div>

                    <div class="overlay-panel overlay-right">
                        <h1>你好, 朋友!</h1>
                        <p>输入您的个人详细信息</p>
                        <p>开始与我们的旅程</p>
                        <button class="ghost" @click="ShowSignUp">去注册</button>
                    </div>
                </div>
            </div>

        </div>
    </div>
</template>

<style scoped>

@import url('https://fonts.googleapis.com/css2?family=Poppins:wght@300;400;700&display=swap');
@import url('https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.4/css/all.min.css');
@import '../static/RegisterAndLogin.css';
</style>
