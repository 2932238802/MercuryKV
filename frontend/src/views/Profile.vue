<script setup>
import { ref, onMounted, computed } from 'vue'; // 引入 computed
import { useRouter } from 'vue-router';
import { ShowCustomModal } from '../components/show';
import service from '../components/request';

const router = useRouter();
const cur_email = ref('');
const cur_user_name = ref('');
const original_email = ref('');
const ismodalvisible = ref(false);
const verificationCode = ref('');
const apiError = ref(''); // 用于显示弹窗内的错误信息
const isSaving = ref(false);
const isSendingCode = ref(false);
const countdown = ref(0);



const HandleReturn = () => {
    router.push({ name: 'Index' });
};
const HandleSaveChangesClick = () => {
    ismodalvisible.value = true;
};
// 关闭窗口
const closeModal = () => {
    ismodalvisible.value = false;
};
const API_PATHS = {
    VERIFYANDMODIFY: '/EmailVerify/verifyandmodify',
    SENDEMAIL: '/EmailVerify/sendemail'
};


/**
 * @brief 发送验证码
 */
const HandleSendCode = () => {
    if (countdown.value > 0) {
        return;
    }

    console.log("将发送验证码到:", original_email.value);

    // TODO: 假设API调用成功...
    // 这里发送 邮件
    const sendemailinfo = {

    };

    try {
        const response = await service.post(


        )
    }
    catch (error) {

    }



    countdown.value = 60;
    const timer = setInterval(() => {
        countdown.value--;
        if (countdown.value <= 0) {
            clearInterval(timer);
        }
    }, 1000);
};

/**
 * @brief
 * 输入完验证码之后 就可以上交了
 * 后端处理 对应的邮箱 账号 和 修改密码
 */
const HandleConfirmAndSave = () => {
    console.log("确认保存！提交的数据:", {
        username: cur_user_name.value,
        email: cur_email.value,
        code: verificationCode.value
    });

};

const sendCodeButtonText = computed(() => {
    if (countdown.value > 0) return `Resend in (${countdown.value}s)`;
    return '发送验证码';
});

onMounted(() => {
    const email = localStorage.getItem("Email");
    const userName = localStorage.getItem("UserName");

    if (!email || !userName) {
        ShowCustomModal("数据本地读取错误!");
        return;
    }
    cur_email.value = email;
    cur_user_name.value = userName;
    original_email.value = email;
});

</script>

<template>
    <div class="page-container">
        <div class="content-card">
            <button class="btn-return" @click="HandleReturn" title="返回首页~">
                <i class="fas fa-arrow-left"></i> <!-- 返回的图标 -->
            </button>

            <header class="card-header">
                <h1 class="title">Profile Settings</h1>
            </header>

            <form @submit.prevent="HandleSaveChangesClick">
                <section class="form-section">
                    <h2 class="section-title">Personal Information</h2>
                    <div class="form-group">
                        <label for="username">Username</label>
                        <input type="text" id="username" class="form-input" v-model="cur_user_name">
                    </div>
                    <div class="form-group">
                        <label for="email">Email Address</label>
                        <input type="email" id="email" class="form-input" v-model="cur_email">
                    </div>
                </section>

                <section class="form-section">
                    <h2 class="section-title">Change Password</h2>
                    <div class="form-group"> <!-- 修改密码的地方 -->
                        <label for="new-password">New Password</label>
                        <input type="password" id="new-password" class="form-input" placeholder="Enter new password">
                    </div>
                </section>

                <footer class="form-footer">
                    <button type="submit" class="btn btn-primary">Save Changes</button>
                </footer>
            </form>

            <section class="danger-zone">
                <div>
                    <h3 class="danger-title">Delete Account</h3>
                    <p class="danger-text">Once you delete your account, there is no going back. Please be certain.</p>
                </div>
                <button class="btn btn-danger">Delete My Account</button>
            </section>
        </div>

        <div v-if="ismodalvisible" class="verification-overlay">
            <div class="verification-modal">
                <i class="fas fa-envelope-open-text modal-icon"></i>
                <h2>Confirm Changes</h2>
                <p class="info-text">
                    For your security, please enter the code sent to your <strong>current email address</strong> ({{
                        original_email }}).
                </p>

                <div class="input-group">
                    <input v-model="verificationCode" type="text" class="form-input" placeholder="Enter code"
                        maxlength="6" />
                    <button @click="HandleSendCode" class="btn btn-ghost" :disabled="isSendingCode || countdown > 0">
                        {{ sendCodeButtonText }}
                    </button>
                </div>

                <p v-if="apiError" class="api-error-text">{{ apiError }}</p>

                <div class="modal-actions">
                    <button @click="closeModal" class="btn btn-ghost">Cancel</button>
                    <button @click="HandleConfirmAndSave" class="btn btn-primary" :disabled="isSaving">
                        <span v-if="isSaving">Saving...</span>
                        <span v-else>Confirm & Save</span>
                    </button>
                </div>
            </div>
        </div>
    </div>
</template>

<style scoped>
@import url('../static/Common.css');
@import url('../static/Profile.css');
</style>
