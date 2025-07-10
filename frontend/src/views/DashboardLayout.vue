<template>
  <div class="dashboard-layout">
    <Sidebar />
    <main class="main-content">
      <router-view />
    </main>
  </div>
</template>

<script setup>
import Sidebar from './Sidebar.vue'; 



</script>

<style scoped>
.dashboard-layout {
  display: flex;
  background-color: #f4f7f6;
  min-height: 100vh;
}
.main-content {
  flex-grow: 1;
  padding-left: 240px; 
}
</style>
