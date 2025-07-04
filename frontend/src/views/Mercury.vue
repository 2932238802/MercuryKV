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
      const index = KvPairs.value.findIndex(item => item.kv_id === currentitem.value.kv_id);
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
      KvPairs.value = KvPairs.value.filter(item => item.kv_id !== kv_id_to_delete);

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
    ShowCustomModal(" 数据加载失败 ");
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
              placeholder="e.g., 12345678, xyz-abc-123..." required></textarea>
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

.kv-manager-page {
  display: flex;
  justify-content: center;
  align-items: flex-start;
  min-height: 100vh;
  padding: 3rem 1rem;
  background: #1f1f1f;
  font-family: 'Poppins', 'Noto Sans SC', sans-serif;
  box-sizing: border-box;
}

.content-card {
  position: relative;
  width: 100%;
  max-width: 1200px;
  background-color: #ffffff;
  border-radius: 12px;
  box-shadow: 0 5px 25px rgba(0, 0, 0, 0.4);
  padding: 2rem 2.5rem;
}

.btn-return {
  position: absolute;
  top: 2rem;
  left: -4rem;
  background: rgba(255, 255, 255, 0.1);
  border: 1px solid rgba(255, 255, 255, 0.2);
  color: white;
  width: 48px;
  height: 48px;
  padding: 0;
  border-radius: 50%;
  font-size: 1.2rem;
  display: flex;
  justify-content: center;
  align-items: center;
}

.btn-return:hover {
  background: rgba(255, 255, 255, 0.2);
  transform: scale(1.05);
}

h1 {
  margin: 0;
  font-weight: 700;
  font-size: 2.2em;
  color: #333;
}

/* --- 头部和工具栏 --- */
.kv-manager-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 2rem;
  padding-left: 2rem;
  /* 为标题留出空间，避免与返回按钮重叠 */
  flex-wrap: wrap;
  gap: 1rem;
}

.toolbar {
  display: flex;
  align-items: center;
  gap: 1rem;
}

.search-input {
  background-color: #f7f7f7;
  border: 1px solid #f0f0f0;
  border-radius: 8px;
  padding: 12px 15px;
  min-width: 300px;
  transition: all 0.3s ease;
}

.search-input:focus {
  outline: none;
  border-color: #ccc;
  background-color: #fff;
}

/* --- 按钮 (核心风格) --- */
button {
  padding: 12px 35px;
  border-radius: 50px;
  font-size: 12px;
  font-weight: bold;
  letter-spacing: 1px;
  text-transform: uppercase;
  cursor: pointer;
  transition: all 0.2s ease-in-out;
  border: 1px solid #111;
}

button:hover {
  transform: scale(1.05);
}

button:active {
  transform: scale(1);
}

button:focus {
  outline: none;
}

button:disabled {
  cursor: not-allowed;
  opacity: 0.6;
}

.btn-primary {
  background-color: #111;
  color: #FFFFFF;
}

.btn-primary:hover {
  background-color: #000;
}

.btn-ghost {
  background-color: transparent;
  border-color: #ccc;
  color: #555;
}

.btn-ghost:hover:not(:disabled) {
  background-color: #f7f7f7;
  color: #111;
}

/* --- 表格 --- */
.table-wrapper {
  overflow-x: auto;
}

.kv-table {
  width: 100%;
  border-collapse: collapse;
  text-align: left;
  color: #333;
}

.kv-table th {
  padding: 1rem 1.5rem;
  background-color: #f9fafb;
  font-size: 0.8rem;
  font-weight: 600;
  text-transform: uppercase;
  letter-spacing: 0.05em;
  color: #666;
  border-bottom: 2px solid #f0f0f0;
}

.kv-table td {
  padding: 1rem 1.5rem;
  border-bottom: 1px solid #f0f0f0;
  vertical-align: middle;
  font-size: 0.9rem;
}

.kv-table tbody tr:last-child td {
  border-bottom: none;
}

.kv-table tbody tr:hover {
  background-color: #fcfcfc;
}

.value-cell {
  font-family: 'SF Mono', 'Courier New', Courier, monospace;
  max-width: 300px;
  /* 防止过长的值撑破布局 */
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}

.value-text {
  color: #059669;
}

.tag {
  display: inline-block;
  padding: 0.25rem 0.75rem;
  background-color: #f0f0f0;
  color: #555;
  border-radius: 50px;
  font-size: 0.8rem;
  font-weight: 500;
  margin-right: 0.5rem;
  margin-bottom: 0.25rem;
}

.actions-cell {
  display: flex;
  gap: 0.5rem;
}

.btn-icon {
  background: none;
  border: none;
  cursor: pointer;
  padding: 0.5rem;
  color: #999;
  font-size: 1rem;
  line-height: 1;
  border-radius: 50%;
  width: 32px;
  height: 32px;
  display: inline-flex;
  justify-content: center;
  align-items: center;
  transition: all 0.2s ease;
}

.btn-icon:hover {
  color: #333;
  background-color: #f0f0f0;
}

.btn-icon.btn-danger:hover {
  color: #dc2626;
  background-color: #fee2e2;
}

.empty-state {
  text-align: center;
  padding: 4rem;
  color: #999;
  font-size: 1rem;
}

.empty-state a {
  color: #555;
  font-weight: 600;
  text-decoration: none;
}

.empty-state a:hover {
  text-decoration: underline;
}

/* --- 页脚和分页 --- */
.kv-manager-footer {
  margin-top: 2rem;
  display: flex;
  justify-content: flex-end;
}

.pagination {
  display: flex;
  align-items: center;
  gap: 0.75rem;
  font-size: 0.9rem;
  color: #888;
}

/* --- 弹窗 (Modal) --- */
.modal-overlay {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background-color: rgba(0, 0, 0, 0.7);
  display: flex;
  justify-content: center;
  align-items: center;
  z-index: 1000;
}

.modal-content {
  width: 100%;
  max-width: 550px;
}

.modal-content form {
  background-color: #FFFFFF;
  display: flex;
  align-items: center;
  justify-content: center;
  flex-direction: column;
  padding: 2.5rem 3rem;
  border-radius: 12px;
  text-align: center;
  box-shadow: 0 5px 25px rgba(0, 0, 0, 0.4);
}

.modal-content h1 {
  margin-bottom: 2rem;
}

.modal-content .form-group {
  width: 100%;
  margin-bottom: 1rem;
}

.modal-content input,
.modal-content textarea {
  background-color: #f7f7f7;
  border: 1px solid #f0f0f0;
  border-radius: 8px;
  padding: 12px 15px;
  margin: 0;
  width: 100%;
  box-sizing: border-box;
  transition: all 0.3s ease;
}

.modal-content input:focus,
.modal-content textarea:focus {
  outline: none;
  border-color: #ccc;
  background-color: #fff;
}

.modal-content textarea {
  resize: vertical;
  min-height: 120px;
}

.modal-actions {
  margin-top: 1.5rem;
  display: flex;
  justify-content: center;
  width: 100%;
  gap: 1rem;
}

.modal-actions button {
  flex-grow: 1;
}
</style>
