import { createRouter, createWebHistory } from 'vue-router'


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
    path:'/Mercury',
    name:'Mercury',
    component:()=>import('../views/Mercury.vue')
  }
]
const router = createRouter({
  history: createWebHistory(), 
  routes: routes
})

export default router
