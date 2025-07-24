<script setup>
import { ref } from 'vue';
import { useRouter } from 'vue-router';
import { ShowCustomModal } from '@/utils/show';
import { SENDCODE, MODIFY_PASSWORD } from '@/utils/api';
const router = useRouter();
const email = ref('');
const newpassword = ref('');
const verificationCode = ref('');
const issendingcode = ref(false);
const countdown = ref(60);

const HandleSendCode = async () => {
  if (!email.value) {
    ShowCustomModal('请输入您的邮箱地址！');
    return;
  }
  if (!/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email.value)) {
    ShowCustomModal('请输入有效的邮箱格式！');
    return;
  }
  issendingcode.value = true;
  try {
    const res = await SENDCODE(email.value);
    const timer = setInterval(() => {
      countdown.value--;
      if (countdown.value <= 0) {
        clearInterval(timer);
        issendingcode.value = false;
        countdown.value = 60;
      }
    }, 1000);
  } catch (error) {
    ShowCustomModal(`发送失败: ${error.message}`);
    issendingcode.value = false;
  }
};

const HandleSubmit = async () => {
  if (!email.value || !newpassword.value || !verificationCode.value) {
    ShowCustomModal('请填写所有字段！');
    return;
  }
  try {
    const info = {
      "email": email.value,
      "password": newpassword.value,
      "code": verificationCode.value,
    }
    const res = await MODIFY_PASSWORD(info);
    if (res.code == 200) {
      ShowCustomModal(res.message);
      router.push({ name: 'RegisterAndLogin' });
    }
  }
  catch (error) {
    ShowCustomModal(`修改失败: ${error.message}`);
  }
};
const GoBack = () => {
  router.back();
}
</script>

<template>
  <div class="forget-password-container">
    <div class="form-wrapper">
      <div class="return-button" @click="GoBack"> &lt; 返回 </div>
      <h1>重置密码</h1>
      <form @submit.prevent="HandleSubmit">
        <div class="input-group">
          <input type="email" v-model="email" placeholder="请输入注册邮箱" required />
        </div>
        <div class="input-group with-button">
          <input type="text" v-model="verificationCode" placeholder="请输入邮箱验证码" required />
          <button type="button" @click="HandleSendCode" :disabled="issendingcode">
            {{ issendingcode ? `${countdown}s 后重发` : '获取验证码' }}
          </button>
        </div>
        <div class="input-group">
          <input type="password" v-model="newpassword" placeholder="请输入新密码" required />
        </div>
        <button type="submit" class="submit-btn">确认修改</button>
      </form>
    </div>
  </div>
</template>
<style scoped>
@import "@/static/ForgetPassword.css";
@import url('https://fonts.googleapis.com/css2?family=Poppins:wght@300;400;700&display=swap');
</style>