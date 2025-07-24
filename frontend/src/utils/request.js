import axios from "axios";
import { ShowCustomModal } from './show';

const service = axios.create({
  baseURL: "/api/",
  timeout: 5000,
});

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
    if ( res.code !== 201 && res.code !== 200) {
      return Promise.reject(new Error(res.message || "Error"));
    } else {
      return res;
    }
  },
  (error) => {
    let message = "发生未知错误";
    if (error) {
      message= error.message || "服务器错误";
    }
    else {
      message = "网络连接中断，请稍后重试";
    }
    console.error("HTTP错误: " + message, error);
    return Promise.reject(error);
  }
);

export default service;

