<script setup>
import { useRouter, useRoute } from 'vue-router';
const router = useRouter();
const route = useRoute();

const ToOther = (pathname) => {
  router.push({ name: pathname })
}

</script>

<template>
  <aside class="app-sidebar">
    <div class="sidebar-header">
      <h2 class="sidebar-title">Mercury</h2>
    </div>
    <nav class="sidebar-nav">
      <ul>
        <li> <!-- 这个是kv 数据表 -->
          <div class="nav-link" :class="{ 'router-link-active': route.name === 'Mercury' }" @click="ToOther('Mercury')">
            <i class="fas fa-database nav-icon"></i> <!-- 图标 -->
            <span>KV Store</span>
          </div>
        </li>

        <li> <!-- apikey -->
          <div class="nav-link" :class="{ 'router-link-active': route.name === 'ApiKeys' }" @click="ToOther('ApiKeys')">
            <i class="fas fa-key nav-icon"></i>
            <span>API Keys</span>
          </div>
        </li>

        <li>
          <div class="nav-link" :class="{ 'router-link-active': route.name === 'Profile' }" @click="ToOther('Profile')">
            <i class="fas fa-user-cog nav-icon"></i>
            <span>Profile Settings</span>
          </div>
        </li>


      </ul>
    </nav>
    <div class=" sidebar-footer">
              <p> &copy; LosAngelous</p>
          </div>
  </aside>
</template>


<style scoped>
@import url('https://fonts.googleapis.com/css2?family=Poppins:wght@300;400;600;700&display=swap');
@import url('https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.4/css/all.min.css');
@import "../static/Sidebay.css";
</style>
