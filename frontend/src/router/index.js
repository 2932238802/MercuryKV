import { createRouter, createWebHistory } from 'vue-router'
import DashboardLayout from '../views/DashboardLayout.vue';

// ! routes 提供给路由
const routes = [
  {
    path: '/',          
    name: 'Index',       
    component:() => import('../views/Index.vue')
  },
  {
    path: '/RegisterAndLogin',
    name: 'RegisterAndLogin',
    component:() => import('../views/RegisterAndLogin.vue')
  },
  {
    path: '/About',
    name: 'About',
    component:() => import('../views/About.vue')
  },
  {
    path: '/',
    component:DashboardLayout,
    children: [
       {
        path: 'Mercury', 
        name: 'Mercury',
        component:()=>import('../views/Mercury.vue')
      },
      {
        path: 'ApiKeys',
        name: 'ApiKeys',
        component: () => import('../views/ApiKeys.vue'), 
      },
      {
        path: 'Profile',
        name: 'Profile',
        component: () => import('../views/Profile.vue'),
      }
    ]
  },

]
const router = createRouter({
  history: createWebHistory(), 
  routes: routes
})

export default router
