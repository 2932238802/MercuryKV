// 推荐的、更健壮的 service.js 版本

import axios from "axios";
import { ShowCustomModal } from '../components/show';

const service = axios.create({
  baseURL: "/",
  timeout: 5000,
});
// const router = useRouter();

// 请求拦截器 (您的版本已经很好了)
service.interceptors.request.use(
  (config) => {
    const token = localStorage.getItem("AuthToken");
    if (token) {
      config.headers["Authorization"] = "Bearer " + token;
    }
    return config;
  },
  (error) => {
    console.error("请求配置错误: ", error);
    return Promise.reject(error);
  }
);

service.interceptors.response.use(
  (response) => {
    const res = response.data;
    // 根据你和后端约定的成功代码进行判断
    if (res.code !== 201 && res.code !== 200) {
      // 显示后端的业务错误信息
      ShowCustomModal(res.message || "业务处理失败");
      return Promise.reject(new Error(res.message || "Error"));
    } else {
      return res;
    }
  },
  (error) => {
    let message = "发生未知错误";
    if (error.response) {
      message= error.response.data.message || "服务器错误";
    }
    else if (error.message.includes('timeout')) {
      message = "请求超时，请检查您的网络连接";
    } 
    else {
      message = "网络连接中断，请稍后重试";
    }
    console.error("HTTP错误: " + message, error);
    ShowCustomModal(message);
    return Promise.reject(error);
  }
);

export default service;

