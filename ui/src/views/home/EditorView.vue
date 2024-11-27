<script setup lang="ts">
    import { env } from '@/utils/env';
    import { ref } from 'vue';
    import CircularLoader from '@/components/CircularLoader.vue';

    const sqlQuery = ref('');

    const loading = ref(false);
    const error = ref();
    const cols = ref([]);
    const data = ref([]);

    async function query() {
        loading.value = true;
        const token = localStorage.getItem(env.localStorageTokenKey) 
        const res = await fetch(`${env.apiURL}/api/query`, {
            method: "POST",
            headers: {
                Authorization: `Bearer ${token}` 
            },
            body: JSON.stringify({query: sqlQuery.value})
        })
        if(res.ok) {
            const {columns, data, error} = await res.json();
            cols.value = columns;
            data.value = data;
            error.value = error;
        }
        loading.value = false;
    }
</script>

<template>
    <div class="main-container">
        <textarea v-model="sqlQuery" name="sqlQuery"></textarea>
        <button class="primary" @click="query">run</button>
        <div class="content">
            <CircularLoader v-if="loading" size="60px" color="#e74c3c" thickness="6px" speed="0.8s"></CircularLoader>
            <table v-else-if="cols && cols.length">
                <thead>
                    <tr>
                    <th v-for="col of cols">
                        {{ col }}
                    </th>
                    </tr>
                </thead>
                <tbody>
                    <tr v-for="d of data">
                        <td v-for="col of cols">
                            {{ d[col] }}
                        </td>
                    </tr>
                </tbody>
            </table>
            <pre v-else-if="error" class="error">{{ error }}</pre>
        </div>
    </div>
</template>

<style scoped>

.main-container{
    display: flex;
    flex-direction: column;
    padding: 1rem;
    gap: 1rem;
    
    .content {

        display: flex;
        align-items: flex-start;
        justify-content: center;
        height: 100%;

        table {
            width: 100%;
        }

        .error{
            color: red;
        }
    }

}

</style>