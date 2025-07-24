import { createRouter, createWebHistory } from "vue-router";
import DashboardLayout from "../views/mercury/DashboardLayout.vue";

// ! routes 提供给路由
const routes = [
  {
    path: "/",
    name: "Index",
    component: () => import("../views/Index.vue"),
  },
  {
    path: "/RegisterAndLogin",
    name: "RegisterAndLogin",
    children: [
      {
        path: "ForgetPassword",
        name: "ForgetPassword",
        component: () => import("../views/LR/ForgetPassword.vue"),
      },
    ],
    component: () => import("../views/LR/RegisterAndLogin.vue"),
  },
  {
    path: "/About",
    name: "About",
    component: () => import("../views/About/About.vue"),
  },

  {
    path: "/",
    component: DashboardLayout,
    children: [
      {
        path: "Mercury",
        name: "Mercury",
        component: () => import("../views/mercury/Mercury.vue"),
      },
      {
        path: "ApiKeys",
        name: "ApiKeys",
        component: () => import("../views/mercury/ApiKeys.vue"),
      },
      {
        path: "Profile",
        name: "Profile",
        component: () => import("../views/mercury/Profile.vue"),
      },
    ],
  },
];
const router = createRouter({
  history: createWebHistory(),
  routes: routes,
});

export default router;
