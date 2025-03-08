<script setup lang="ts">


import { Input } from '@/components/ui/input';
import { Button } from '@/components/ui/button';
import Table from '@/components/Table.vue'

import { Users, Folder, Play } from 'lucide-vue-next'
import InsertRow from "@/components/InsertRow.vue"
import EditTable from '@/components/EditTable.vue'
import { onMounted, ref } from 'vue';
import CreateTable from "@/components/CreateTable.vue";
import { cbFetch } from "@/services/api-service";
import router from '@/router';
import type { Cols } from '@/utils/types';

const list = ref<string[]>([]);
const search = ref<string>("");


const whereClause = ref<string>();
const currentTable = ref<string>("user");

const cols = ref<Cols[]>([]);
const data = ref<{ [key: string]: string }[]>([]);

async function queryTable(table: string, where?: string) {
    let query = `/api/table/${table}`;
    if (where) query += `?where=${where}`;
    const res = await cbFetch(query)
    cols.value = res.columns;
    data.value = res.data;
    currentTable.value = table;
    router.push(table);
}

async function fecthTables() {
    const res = await cbFetch('/api/tables');
    list.value = res.data.map((v: any) => { return v.name });
}

onMounted(async () => {
    await fecthTables();
    //await queryTable(router.currentRoute.value.fullPath.split('/').pop() || 'users');
})

</script>

<template>
    <div class="flex flex-row h-full">
        <div class="flex flex-col rounded-tr-2xl p-4 gap-4 items-start bg-secondary text-secondary-foreground">
            <span class="flex flex-row gap-4 items-center cursor-pointer"
                v-for="table of list.filter((t: string) => t.includes(search))" @click="queryTable(table)">
                <Users v-if="table == 'users'" />
                <Folder v-else />
                {{ table }}
            </span>
            <span class="self-center">
                    <CreateTable @created="fecthTables"></CreateTable>
            </span>
        </div>

        <div class="flex gap-4 flex-col pt-0 p-4 w-full">

            <template v-if="cols && cols.length > 0">
                <div class="flex w-full items-center gap-1.5">

                    <EditTable @deleted="fecthTables" :tableName="currentTable" :cols="cols"></EditTable>
                    <InsertRow :table-name="currentTable" :cols="cols"></InsertRow>

                    <Input type="text" v-model="whereClause" placeholder="Where clause" />
                    <Button variant="outline" @click="queryTable(currentTable, whereClause)">
                        <Play />
                    </Button>
                </div>

                <Table :cols="cols" :data="data"></Table>
            </template>
        </div>

    </div>
</template>

<style scoped></style>