<script setup lang="ts">
import { isModalVisible, modalMessage, CloseCustomModal } from './components/show'; 
</script>

<template>
  <router-view />
  <div v-if="isModalVisible" class="modal-overlay" @click.self="CloseCustomModal">
    <div class="modal-content">
      <p class="modal-message" v-html="modalMessage.replace(/\n/g, '<br>')"></p>
      <button class="modal-close-button" @click="CloseCustomModal">关闭</button>
    </div>
  </div>
</template>

<style>
body,
html {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

.modal-overlay {
  position: fixed;
  top: 0;
  left: 0;
  width: 100vw;
  height: 100vh;
  background-color: rgba(0, 0, 0, 0.65);
  display: flex;
  justify-content: center;
  align-items: center;
  z-index: 10000;
  padding: 20px;
  box-sizing: border-box;
}

.modal-content {
  background-color: #ffffff;
  color: #333333;
  padding: 30px 40px;
  border-radius: 10px;
  box-shadow: 0 8px 25px rgba(0, 0, 0, 0.15);
  min-width: 320px;
  max-width: 500px;
  width: auto;
  text-align: center;
  border: 1px solid #e0e0e0;
  animation: fadeInScale 0.3s ease-out;
}

@keyframes fadeInScale {
  from {
    opacity: 0;
    transform: scale(0.95);
  }

  to {
    opacity: 1;
    transform: scale(1);
  }
}

.modal-message {
  margin-bottom: 25px;
  font-size: 1.05em;
  line-height: 1.65;
  color: #2c3e50;
  white-space: pre-line;
}

.modal-close-button {
  background-color: #4A4A4A;
  color: white;
  border: none;
  padding: 12px 30px;
  border-radius: 6px;
  cursor: pointer;
  font-size: 1em;
  font-weight: 500;
  transition: background-color 0.2s ease-in-out, transform 0.1s ease;
  min-width: 100px;
}

.modal-close-button:hover {
  background-color: #303030;
}

.modal-close-button:active {
  transform: scale(0.97);
}
</style>
