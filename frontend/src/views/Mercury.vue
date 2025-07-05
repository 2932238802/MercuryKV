<script setup>
import { ref, computed, onMounted } from 'vue';
import { useRouter } from 'vue-router';
import service from '../components/request';
import { ShowCustomModal } from '../components/show';

// ----------------- ------------------------------------------------
const KvPairs = ref([]);
const ismodalopen = ref(false);
const isediting = ref(false);
const currentitem = ref(null);
const editabletags = ref('');
const router = useRouter();
const DATA_ALTER_METHOD = {
  CREATE: 'create',
  UPDATE: 'update',
};
const API_PATH = {
  FETCH: "/FetchData/fetch",
  CREATE: "/Alter/adddata",
  UPDATE: "/Alter/alterdata",
  DELETE: "/Alter/deletedata",
};
const data_alter_method = ref(DATA_ALTER_METHOD.CREATE);
const modaltitle = computed(() => {
  return isediting.value ? 'Edit KV Pair' : 'Create KV Pair';
});
// -----------------------------------------------------------------

// -----------------------------------------------------------------
// 导航到主页或上一页
function HandleReturn() {
  router.push({ name: 'Index' });
}

// -----------------------------------------------------------------

// -----------------------------------------------------------------
const HandleReLogin = () => {
  // 本地存放 token
  localStorage.removeItem('AuthToken');
  localStorage.removeItem('UserId');
  router.push({ name: 'RegisterAndLogin' });
}
// -----------------------------------------------------------------

// -----------------------------------------------------------------
// 打开“添加”弹窗
const HandleAddNew = ()=> {
  isediting.value = false;

  console.log("进入添加的窗口...");

  currentitem.value = {
    value_input: '',
    key_input: '',
    tags: [],
  };
  editabletags.value = '';

  data_alter_method.value = DATA_ALTER_METHOD.CREATE;

  ismodalopen.value = true;
}
// -----------------------------------------------------------------

// -----------------------------------------------------------------
// 打开“编辑”弹窗
function HandleEdit(itemtoedit) {
  isediting.value = true;
  currentitem.value = JSON.parse(JSON.stringify(itemtoedit));
  editabletags.value = itemtoedit.tags ? itemtoedit.tags.join(', ') : '';
  data_alter_method.value = DATA_ALTER_METHOD.UPDATE;
  ismodalopen.value = true;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// 关闭弹窗并重置状态
const CloseModal = () =>{
  ismodalopen.value = false;
  currentitem.value = null;
  editabletags.value = '';
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// 处理表单提交（添加或更新）
const HandleSubmit = async () => {
  if (!currentitem.value) return;
  const user_id = localStorage.getItem("UserId");
  if (!user_id) {
    ShowCustomModal("User not logged in Please re-login");
    HandleReLogin();
    return;
  }
  if (currentitem.value.key_input.trim() === "" || currentitem.value.value_input.trim() === "") {
    ShowCustomModal("Key and Value cannot be empty!");
    return;
  }

  // 分解标签
  const separator = /[,\s，。.!-]+/;
  const tags_array = [...new Set(
    editabletags.value
      .split(separator)
      .filter(tag => tag.trim().length > 0)
  )];

  try {
    if (data_alter_method.value === DATA_ALTER_METHOD.CREATE) {
      const post_info = {
        "user_id": Number(user_id),
        "key_input": currentitem.value.key_input,
        "value_input": currentitem.value.value_input,
        "tags": tags_array,
      };
      const response = await service.post(API_PATH.CREATE, post_info);
      console.log('API /Alter/adddata returned:', response.data);
      KvPairs.value.unshift(response.data);
      ShowCustomModal("Item created successfully!");
    } 
    else if (data_alter_method.value === DATA_ALTER_METHOD.UPDATE) {
      // 编辑信息
      const post_info = {
        "user_id": Number(user_id),
        "kv_id": currentitem.value.kv_id,
        "key_input": currentitem.value.key_input,
        "value_input": currentitem.value.value_input,
        "tags": tags_array,
      };
      const response = await service.put(API_PATH.UPDATE, post_info);
      const index = KvPairs.value.findIndex(item => item.kv_id == currentitem.value.kv_id);
      if (index !== -1) {
        KvPairs.value[index] = response.data;
      }
      ShowCustomModal("Item updated successfully!");
    }
  } catch (error) {
    console.error("Submission failed:", error);
    ShowCustomModal(error.response?.data?.message || error.message || "An unknown error occurred.");
  }
  CloseModal();
}

// -----------------------------------------------------------------

// -----------------------------------------------------------------
// 删除条目 (带确认)
const HandleDelete = async (itemtodelete) => {
  if (confirm(`你确定要删除这个"${itemtodelete.key_input}"?`)) {
    try {
      const kv_id_to_delete = itemtodelete.kv_id;

      console.log(`删除的kv_id是: ${itemtodelete.kv_id}`)
      console.log(`ID to delete is: ${kv_id_to_delete}, type is: ${typeof kv_id_to_delete}`);
      console.log('First item in array has ID type:', typeof KvPairs.value[0]?.kv_id);
      // --------------------------

      const res = await service.delete(`${API_PATH.DELETE}/${kv_id_to_delete}`);
      KvPairs.value = KvPairs.value.filter(item => item.kv_id != kv_id_to_delete);

      ShowCustomModal(res.message);
    }
    catch (error) {
      console.error("Deletion failed:", error);
      ShowCustomModal(error.message || "Failed to delete item.");
    }
  }
}
// -----------------------------------------------------------------

const loadInitialData = async () => {
  const user_id = localStorage.getItem("UserId");
  if (!user_id) {
    HandleReLogin();
    return;
  }
  try {
    const response = await service.get(API_PATH.FETCH, { params: { user_id } });
    KvPairs.value = response.data || [];
  } catch (error) {
    console.error("Failed to fetch data:", error);
  }
};

onMounted(() => {
  loadInitialData();
});
</script>

<template>
  <!-- A: 页面根容器 -->
  <div class="kv-manager-page">

    <!-- B_1: 主要内容卡片 -->
    <div class="content-card">

      <!-- C_1: 返回按钮 -->
      <button class="btn-return" @click="HandleReturn" title="Go back">
        <i class="fas fa-arrow-left"></i>
      </button>

      <!-- C_2: 头部和工具栏 -->
      <header class="kv-manager-header">
        <h1>KV Store</h1>
        <div class="toolbar">
          <input type="text" class="search-input" placeholder="Search by key or tag..."
            aria-label="Search Key-Value pairs" />
          <button class="btn-primary" @click="HandleAddNew">+ Add New</button>
          <button class="btn-ghost" @click="HandleReLogin" title="Logout and login again">
            <i class="fas fa-sign-out-alt"></i> Re-Login
          </button>
        </div>
      </header>

      <!-- C_3: 数据表格 -->
      <div class="table-wrapper">
        <table class="kv-table">
          <thead>
            <tr>
              <th class="col-key">Key</th>
              <th class="col-value">Value</th>
              <th class="col-tags">Tags</th>
              <th class="col-updated">Last Updated</th>
              <th class="col-actions">Actions</th>
            </tr>
          </thead>
          <tbody>
            <!-- 使用 v-for 循环渲染 KvPairs 数组 -->
            <tr v-for="item in KvPairs" :key="item.kv_id">
              <td>{{ item.key_input }}</td>
              <td class="value-cell">
                <span class="value-text">{{ item.value_input }}</span>
              </td>
              <td>
                <span v-for="tag in item.tags" :key="tag" class="tag">
                  {{ tag }}
                </span>
                <span v-if="!item.tags || item.tags.length === 0" style="color: #999;">-</span>
              </td>
              <td>{{ new Date(item.updated_at).toLocaleString() }}</td>
              <td class="actions-cell">
                <button class="btn-icon" title="View History" @click="handleViewHistory(item)">
                  <i class="fas fa-history"></i>
                </button>
                <button class="btn-icon" title="Edit" @click="HandleEdit(item)">
                  <i class="fas fa-pencil-alt"></i>
                </button>
                <button class="btn-icon btn-danger" title="Delete" @click="HandleDelete(item)">
                  <i class="fas fa-trash-alt"></i>
                </button>
              </td>
            </tr>
            <tr v-if="!KvPairs || KvPairs.length === 0">
              <td colspan="5" class="empty-state">
                No Key-Value pairs found. <a href="#" @click.prevent="HandleAddNew">Add one now!</a>
              </td>
            </tr>
          </tbody>
        </table>
      </div>

      <footer class="kv-manager-footer">
        <div class="pagination">
          <span>Page 1 of 1</span>
          <button class="btn-ghost" disabled>&lt; Prev</button>
          <button class="btn-ghost" disabled>Next &gt;</button>
        </div>
      </footer>
    </div>

    <div class="modal-overlay" v-if="ismodalopen">
      <div class="modal-content">
        <form @submit.prevent="HandleSubmit" v-if="currentitem">
          <h1>{{ modaltitle }}</h1>
          <div class="form-group">
            <label for="kv-key">Key</label>
            <input v-model="currentitem.key_input" id="kv-key" type="text"
              placeholder="e.g., WiFi Password, API Key" required />
          </div>
          <div class="form-group">
            <label for="kv-value">Value</label>
            <textarea v-model="currentitem.value_input" id="kv-value" rows="5"
              placeholder="e.g., 对应key的值..." required></textarea>
          </div>
          <div class="form-group">
            <label for="kv-tags">Tags (separated by comma, space, etc.)</label>
            <input v-model="editabletags" id="kv-tags" type="text" placeholder="e.g., Personal, Work, Project" />
          </div>
          <div class="modal-actions">
            <button type="button" class="btn-ghost" @click="CloseModal">Cancel</button>
            <button type="submit" class="btn-primary">Save</button>
          </div>
        </form>
      </div>
    </div>
  </div>
</template>

<style scoped>
/* 样式部分保持不变 */
@import url('https://fonts.googleapis.com/css2?family=Poppins:wght@300;400;700&display=swap');
@import url('https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.4/css/all.min.css');
@import "../static/Mercury.css";
</style>
