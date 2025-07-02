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
        
        // TODO: 后端看一眼
        localStorage.setItem('AuthToken', response.token);
        localStorage.setItem('UserId', response.user_id);
        router.push({ name: "Mercury" }).catch(err => {
            console.error('路由跳转失败:', err);
            ShowCustomModal(`路由跳转失败: ${err.message}`); 
        });

        ShowCustomModal(response.message || "登录成功");
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
        localStorage.setItem('AuthToken', token);
        localStorage.setItem('UserId', user_id);
        router.push({ name: "Mercury" });
        ShowCustomModal(response.message);                        // TODO: 后端看一眼
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

.page-body {
    display: flex;

    /* 内边距 */
    padding: 20px;

    /* 水平方向上居中对齐 */
    justify-content: center;

    /* 竖直方向上对齐 */
    align-items: center;

    /* 设置元素 元素的高度等于当前浏览器可见窗口高度的 100% */
    min-height: 100vh;

    /* 样式配置 */
    background: #1f1f1f;

    /* 字体设置 */
    font-family: 'Poppins', 'Noto Sans SC', sans-serif;

    /* 
        border-box 后：一个元素的总宽度就是你设置的 width
        默认情况 (content-box)：一个元素的总宽度 = width + padding + border
        这使得布局计算很麻烦 
    */
    box-sizing: border-box;
}

/* 
    登录注册的样式
    就是那几个特别大的字
*/
h1 {
    /* 上右下左 */
    margin: 0 0 1rem 0;

    /* 
        字体粗细 
        字体大小
        字体颜色
    */
    font-weight: 700;
    font-size: 2.2em;
    color: #333;
}


/* 
    内容提示
    为了与我们保持联系，请使用您的个人信息登录
    输入您的个人详细信息，开始与我们的旅程
*/
p {

    font-size: 15px;
    font-weight: 300;

    /* 设置行高  */
    /* letter-spacing 字符间距 */
    line-height: 1.5;
    letter-spacing: 4.0px;
    margin: 5px 0 40px;
}

span {
    font-size: 13px;
    color: #555;
}


/*
    按钮设置
    一个是登录 一个是注册
*/
button {

    /* 边界 */
    margin-top: 30px;
    margin-left: auto;
    margin-right: auto;
    margin-bottom: 100px;
    padding: 12px 45px;

    /* 圆角弧度 */
    border-radius: 50px;
    border: 1px solid #111;

    /* 背景颜色 */
    background-color: #111;

    /* 字体 */
    font-size: 12px;
    font-weight: bold;
    color: #FFFFFF;
    letter-spacing: 1px;

    /* 鼠标悬停 */
    text-transform: uppercase;
    cursor: pointer;

    /* 动画 */
    transition: all 0.2s ease-in-out;
}

button:hover {
    transform: scale(1.05);
    background-color: #000;
}

button:active {
    transform: scale(1);
}

button:focus {
    outline: none;
}

button.ghost {
    background-color: transparent;
    border-color: #FFFFFF;
}

button.ghost:hover {
    background-color: #fff;
    color: #111;
}

form {
    background-color: #FFFFFF;
    display: flex;
    align-items: center;
    justify-content: center;
    flex-direction: column;
    padding: 0 50px;
    height: 100%;
    text-align: center;
}

input {
    background-color: #f7f7f7;
    border: 1px solid #f0f0f0;
    border-radius: 8px;
    padding: 12px 15px;
    margin: 8px 0;
    width: 100%;
    transition: all 0.3s ease;
}

input:focus {
    outline: none;
    border-color: #ccc;
    background-color: #fff;
}

/* 
    登录注册 总的页面
*/
.container {
    /* 位置 和 形状*/
    position: relative;
    border-radius: 12px;

    /* 背景颜色 */
    /* 阴影 */
    background-color: #fff;
    box-shadow: 0 5px 25px rgba(0, 0, 0, 0.4);


    overflow: hidden;
    width: 100%;
    max-width: 790px;
    min-height: 520px;
}


/*
左右两边分栏
满足左右动画效果
*/
.form-container {
    /* 位置 */
    position: absolute;
    top: 0;
    height: 100%;

    /* 动画 */
    transition: all 0.6s ease-in-out;
}



.sign-in-container {
    left: 0;
    width: 50%;
    z-index: 2;
}

.sign-up-container {
    left: 0;
    width: 50%;
    opacity: 0;
    z-index: 1;
}

.container.right-panel-active .sign-in-container {
    transform: translateX(100%);
}

.container.right-panel-active .sign-up-container {
    transform: translateX(100%);
    opacity: 1;
    z-index: 5;
    animation: show 0.6s;
}


.overlay-container {
    position: absolute;
    top: 0;
    left: 50%;
    width: 50%;
    height: 100%;
    overflow: hidden;
    transition: transform 0.6s ease-in-out;
    z-index: 100;
}

.container.right-panel-active .overlay-container {
    transform: translateX(-100%);
}

.overlay {
    background: #111;
    color: #FFFFFF;
    position: relative;
    left: -100%;
    height: 100%;
    width: 200%;
    transform: translateX(0);
    transition: transform 0.6s ease-in-out;
}

.container.right-panel-active .overlay {
    transform: translateX(50%);
}

.overlay-panel {
    position: absolute;
    display: flex;
    align-items: center;
    justify-content: center;
    flex-direction: column;
    padding: 0 0px;
    text-align: center;
    top: 0;
    height: 100%;
    width: 50%;
    transform: translateX(0);
    transition: transform 0.6s ease-in-out;
}

.overlay-panel h1 {
    color: #fff;
}

.overlay-right {
    right: 0;
}





/* 
警告颜色 

*/
.warning_word {
    color: red;
    font-size: 12px;
    margin-top: -5px;
    margin-bottom: 10px;
    text-align: left;
    width: 100%;
}

/*

*/
.return-button {
    display: inline-flex;
    padding: 7px 22px;
    margin-left: 240px;
    margin-top: 50px;

    background-color: #555;
    border: none;
    border-radius: 20px;

    /* 字体 和 内容哈 */
    justify-content: center;
    align-items: center;
    color: #fff;
    font-size: 12px;
    font-weight: bold;
    text-decoration: none;

    /* 交互 */
    /* 明确可点击状态 */
    /* 添加平滑过渡效果 */
    cursor: pointer;
    transition: background-color 0.2s ease;
}

.return-button:hover {
    background-color: #333;
}



/* 动画设置 */
@keyframes show {

    0%,
    49.99% {
        opacity: 0;
        z-index: 1;
    }

    50%,
    100% {
        opacity: 1;
        z-index: 5;
    }
}
</style>
