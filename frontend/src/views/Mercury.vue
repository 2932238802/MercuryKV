<script setup>
import { ref, computed } from 'vue';
import { useRouter } from 'vue-router';

// -----------------------------------------------------------------
const KvPairs = ref([
  { id: 1, key: 'my-app:config:theme', value: '"dark"', tags: ['config', 'ui'], updated_at: '2023-10-27 10:30:15' },
  { id: 2, key: 'user:101:permissions', value: '["read", "write", "comment"]', tags: ['user', 'security'], updated_at: '2023-10-26 18:05:01' },
]);
const ismodalopen = ref(false);
const isediting = ref(false);
const currentitem = ref(null);
const router = useRouter();
const modaltitle = computed(() => {
  return isediting.value ? 'Edit KV Pair' : 'Create KV Pair';
});
// -----------------------------------------------------------------



// -----------------------------------------------------------------
// 当程序需要读取 TagsAsString  值的时候 -> get() 
// 写入或者 修改的时候 才会调用 这个 set
const TagsAsString = computed({
  get() {
    return currentitem.value?.tags?.join(',') || '';
  },
  set(newValue) {
    if (currentitem.value) {
      currentitem.value.tags = newValue.split(/[,\s\.，。]+/)
        .map(tag => tag.trim())
        .filter(tag => tag);
    }
  }
});
// -----------------------------------------------------------------



// -----------------------------------------------------------------
// 导航到主页或上一页
function HandleReturn() {
  router.push({ name: 'Index' }); // 假设你的主页路由名叫 'Index'
}
// -----------------------------------------------------------------



// -----------------------------------------------------------------
function HandleReLogin() {
  localStorage.removeItem('AuthToken');
  router.push({ name: 'RegisterAndLogin' });
}
// -----------------------------------------------------------------



// -----------------------------------------------------------------
// 打开“添加”弹窗
function HandleAddNew() {
  isediting.value = false;
  // 为新条目提供一个干净的模板
  currentitem.value = {
    key: '',
    value: '',
    tags: []
  };
  ismodalopen.value = true;
}
// -----------------------------------------------------------------



// -----------------------------------------------------------------
// 打开“编辑”弹窗
function handleEdit(itemtoedit) {
  isediting.value = true;
  // 创建一个副本进行编辑，避免直接修改列表中的原始数据
  currentitem.value = { ...itemtoedit };
  ismodalopen.value = true;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// 关闭弹窗并重置状态
function CloseModal() {
  ismodalopen.value = false;
  currentitem.value = null;
}
// -----------------------------------------------------------------



// -----------------------------------------------------------------
// 处理表单提交（添加或更新）
async function HandleSubmit() {
  if (!currentitem.value) return;

  if (isediting.value) {
    // 日志输出
    console.log('Updating item:', currentitem.value);

    // 后端实现一下
    // TODO: 提交表单

    const index = KvPairs.value.findIndex(item => item.id === currentitem.value.id);
    if (index !== -1) {
      KvPairs.value[index] = { ...currentitem.value, updated_at: new Date().toLocaleString() };
    }

  } else {
    console.log('Creating new item:', currentitem.value);
    const newItem = { ...currentitem.value, id: Date.now(), updated_at: new Date().toLocaleString() }; // 使用时间戳作为临时id
    KvPairs.value.unshift(newItem);
  }

  CloseModal();
}
// -----------------------------------------------------------------



// -----------------------------------------------------------------
// 删除条目 (带确认)
function HandleDelete(itemtodelete) {
  if (confirm(`Are you sure you want to delete the key "${itemtodelete.key}"?`)) {
    console.log('Deleting item:', itemtodelete);

    //TODO: 后端完善一下
    // filter 就能过滤内容
    KvPairs.value = KvPairs.value.filter(item => item.id !== itemtodelete.id);
  }
}
// -----------------------------------------------------------------
</script>

<template>
  <!-- A -->
  <div class="kv-manager-page">

    <!-- B_1. 主要业务代码 -->
    <div class="content-card">

      <!-- C_1 这个是返回按钮 -->
      <!-- title 当用户将鼠标悬停在按钮上时，会显示一个提示框显示 "Go back"  -->
      <button class="btn-return" @click="HandleReturn" title="Go back">
        <i class="fas fa-arrow-left"></i>
      </button>

      <!-- C_2 工具栏 -->
      <!-- 一个搜索输入框 一个是增加新value -->
      <header class="kv-manager-header">
        <h1>KV Store</h1>

        <div class="toolbar">

          <input type="text" class="search-input" placeholder="Search by key or tag..."
            aria-label="Search Key-Value pairs" />

          <button class="btn-primary" @click="HandleAddNew">
            + Add New
          </button>

          <button class="btn-ghost" @click="HandleReLogin" title="Logout and login again">
            <i class="fas fa-sign-out-alt"></i>
            Re-Login
          </button>
        </div>
      </header>

      <!-- C_3 主要显示栏 -->
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
            <tr v-for="item in KvPairs" :key="item.id">
              <td>{{ item.key }}</td>
              <td class="value-cell">
                <span class="value-text">{{ item.value }}</span>
              </td>
              <td>
                <span v-for="tag in item.tags" :key="tag" class="tag">{{ tag }}</span>
              </td>
              <td>{{ item.updated_at }}</td>
              <td class="actions-cell">
                <button class="btn-icon" title="View History" @click="handleViewHistory(item)">
                  <i class="fas fa-history"></i>
                </button>

                <button class="btn-icon" title="Edit" @click="handleEdit(item)">
                  <i class="fas fa-pencil-alt"></i>
                </button>

                <button class="btn-icon btn-danger" title="Delete" @click="HandleDelete(item)">
                  <i class="fas fa-trash-alt"></i>
                </button>

              </td>
            </tr>

            <!-- 空状态：当没有数据时显示 -->
            <tr v-if="!KvPairs.length">
              <td colspan="5" class="empty-state">
                No Key-Value pairs found. <a href="#" @click.prevent="HandleAddNew">Add one now!</a>
              </td>
            </tr>

          </tbody>
        </table>
      </div>

      <!-- C_4 换页按钮 -->
      <!-- TODO: 页面部分完善 -->
      <footer class="kv-manager-footer">
        <div class="pagination">
          <span>Page 1 of 1</span>
          <button class="btn-ghost" disabled>&lt; Prev</button>
          <button class="btn-ghost" disabled>Next &gt;</button>
        </div>
      </footer>

    </div>

    <!-- B_2. 弹窗 -->
    <div class="modal-overlay" v-if="ismodalopen">
      <div class="modal-content">
        <form @submit.prevent="HandleSubmit">
          <h1>{{ modaltitle }}</h1>
          <div class="form-group">
            <input v-model="currentitem.key" id="kv-key" type="text" placeholder="Key (e.g., app:settings:retries)"
              required />
          </div>
          <div class="form-group">
            <textarea v-model="currentitem.value" id="kv-value" rows="5"
              placeholder="Value (string or valid JSON)"></textarea>
          </div>
          <div class="form-group">
            <input v-model="TagsAsString" id="kv-tags" type="text"
              placeholder="Tags (comma separated, e.g., api,config,user)" />
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
