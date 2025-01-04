<script setup lang="ts">

import Table from '@/components/Table.vue'
    import { Button } from '@/components/ui/button'
    import { Textarea } from '@/components/ui/textarea'

    import { cbFetch } from '@/services/api-service';
    import { onMounted, ref, useTemplateRef } from 'vue';

    const sqlQuery = ref('');

    const loading = ref(false);
    const cols = ref([]);
    const data = ref([]);

    async function query() {
        loading.value = true;
        const res = await cbFetch('/api/admin/query', "POST", {query: sqlQuery.value});
        cols.value = res.columns;
        data.value = res.data;
        loading.value = false;
    }

</script>

<template>
    <div class="main-container p-4">
        
        <Textarea v-model="sqlQuery" name="sqlQuery"></Textarea>
        <div class="actions">
            <Button variant="outline" @click="query">run</Button>
        </div>
        <div class="content" :aria-busy="loading">
            <template v-if="cols && cols.length">
                <Table :cols="cols" :data="data"></Table>
            </template>
        </div>
    </div>
</template>

<style scoped>

.main-container{
    display: flex;
    flex-direction: column;
    gap: 1rem;
    
    .actions{
        display: flex;
    }

    .content {

        display: flex;
        align-items: flex-start;
        justify-content: center;
        height: 100%;
    }

}

</style>