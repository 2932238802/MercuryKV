<script setup>
import { ref, onMounted, computed } from 'vue';
import { useRouter } from 'vue-router';
import { ElMessage } from 'element-plus';
import { SENDCODE, SEND_EMAIL_FORPROFILE } from '@/utils/api.js'
import { ShowCustomModal } from '../../utils/show';

const router = useRouter();
const cur_email = ref('');
const cur_user_name = ref('');
const new_password = ref('');
const original_email = ref('');
const original_username = ref('');
const is_modal_visible = ref(false);
const verification_code = ref('');
const api_error = ref('');
const is_saving = ref(false);
const is_sending_code = ref(false);
const countdown = ref(0);
let countdown_timer = null;

const HandleReturn = () => {
    router.push({ name: 'Index' });
};
const HandleSaveChangesClick = () => {
    if (!cur_user_name.value.trim() || !cur_email.value.trim()) {
        ElMessage.error('用户名和邮箱不能为空');
        return;
    }
    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    if (!emailRegex.test(cur_email.value)) {
        ElMessage.error('请输入有效的邮箱地址');
        return;
    }
    if (new_password.value && new_password.value.length < 6) {
        ElMessage.error('新密码长度不能少于6位');
        return;
    }
    const hasChanges =
        cur_user_name.value !== original_username.value ||
        cur_email.value !== original_email.value ||
        new_password.value.trim() !== '';
    if (!hasChanges) {
        ElMessage.info('信息未发生任何变更');
        return;
    }
    is_modal_visible.value = true;
};

const CloseModal = () => {
    is_modal_visible.value = false;
    verification_code.value = '';
    api_error.value = '';
    if (countdown_timer) {
        clearInterval(countdown_timer);
        countdown.value = 0;
    }
};

const HandleSendCode = async () => {
    if (is_sending_code.value || countdown.value > 0) {
        return;
    }
    is_sending_code.value = true;
    api_error.value = '';
    try {
        console.log(original_email.value);
        await SENDCODE(original_email.value)
        alert("验证码已经发送")
        countdown.value = 60;
        countdown_timer = setInterval(() => {
            countdown.value--;
            if (countdown.value <= 0) {
                clearInterval(countdown_timer);
            }
        }, 1000);
    } catch (error) {
        console.error("发送验证码失败:", error);
        api_error.value = '请求失败，请检查网络连接';
    } finally {
        is_sending_code.value = false;
    }
};

const HandleConfirmAndSave = async () => {
    if (!verification_code.value || verification_code.value.length !== 6) {
        api_error.value = '请输入6位验证码';
        return;
    }
    is_saving.value = true;
    api_error.value = '';
    try {
        const payload = {
            "username": cur_user_name.value,
            "email": cur_email.value,
            "user_code": verification_code.value,
            "password": new_password.value.trim(),
            "user_id": localStorage.getItem("UserId")
        };
        const response = await SEND_EMAIL_FORPROFILE(payload);
        alert("修改成功")
        console.log("Email", response.email);
        console.log("UserName", response.username);
        localStorage.setItem("Email", response.email);
        localStorage.setItem("UserName", response.username);
        original_email.value = cur_email.value;
        original_username.value = cur_user_name.value;
        new_password.value = '';
        CloseModal();
    } catch (error) {
        console.error("修改信息失败:", error);
        api_error.value = '请求失败，请检查网络或联系管理员';
    } finally {
        is_saving.value = false;
    }
};

const SendCodeButtonText = computed(() => {
    if (countdown.value > 0) return `重新发送 (${countdown.value}s)`;
    if (is_sending_code.value) return '发送中...';
    return '发送验证码';
});

onMounted(() => {
    const email = localStorage.getItem("Email");
    const userName = localStorage.getItem("UserName");
    if (!email || !userName) {
        ElMessage.error("本地数据读取错误，请重新登录!");
        router.push({ name: 'Login' });
        return;
    }
    cur_email.value = email;
    cur_user_name.value = userName;
    original_email.value = email;
    original_username.value = userName;
});
</script>

<template>
    <div class="page-container">
        <div class="content-card">
            <button class="btn-return" @click="HandleReturn" title="返回首页~">
                <i class="fas fa-arrow-left"></i>
            </button>
            <header class="card-header">
                <h1 class="title">Profile Settings</h1>
            </header>
            <form @submit.prevent="HandleSaveChangesClick">
                <section class="form-section">
                    <h2 class="section-title">Personal Information</h2>
                    <div class="form-group">
                        <label for="username">Username</label>
                        <input type="text" id="username" class="form-input" v-model="cur_user_name" required>
                    </div>
                    <div class="form-group">
                        <label for="email">Email Address</label>
                        <input type="email" id="email" class="form-input" v-model="cur_email" required>
                    </div>
                </section>
                <section class="form-section">
                    <h2 class="section-title">Change Password</h2>
                    <div class="form-group">
                        <label for="new-password">New Password</label>
                        <input type="password" id="new-password" class="form-input" v-model="new_password"
                            placeholder="留空则不修改密码" minlength="6">
                    </div>
                </section>

                <footer class="form-footer">
                    <button type="submit" class="btn btn-primary">Save Changes</button>
                </footer>
            </form>
        </div>

        <!-- 验证码弹窗 -->
        <div v-if="is_modal_visible" class="verification-overlay">
            <div class="verification-modal">
                <i class="fas fa-envelope-open-text modal-icon"></i>
                <h2>Confirm Changes</h2>
                <p class="info-text">
                    为了您的安全，验证码将发送到您<strong>之前的邮箱</strong> ({{ original_email }}).
                </p>

                <div class="input-group">
                    <input v-model="verification_code" type="text" class="form-input" placeholder="输入6位验证码"
                        maxlength="6" />
                    <button @click="HandleSendCode" class="btn btn-ghost" :disabled="is_sending_code || countdown > 0">
                        {{ SendCodeButtonText }}
                    </button>
                </div>

                <p v-if="api_error" class="api-error-text">{{ api_error }}</p>

                <div class="modal-actions">
                    <button @click="CloseModal" class="btn btn-ghost" :disabled="is_saving">Cancel</button>
                    <button @click="HandleConfirmAndSave" class="btn btn-primary"
                        :disabled="is_saving || verification_code.length !== 6">
                        <span v-if="is_saving">Saving...</span>
                        <span v-else>Confirm & Save</span>
                    </button>
                </div>
            </div>
        </div>
    </div>
</template>

<style scoped>
@import url('@/static/Common.css');
@import url('@/static/Profile.css');

.api-error-text {
    color: #e74c3c;
    min-height: 1.2em;
    margin-top: 10px;
    font-size: 0.9em;
}
</style>
