<script setup lang="ts">

import {
    Table,
    TableBody,
    TableCell,
    TableHead,
    TableHeader,
    TableRow,
} from '@/components/ui/table'
import {
    Dialog,
    DialogTrigger,
} from '@/components/ui/dialog'
import { Button } from '@/components/ui/button';
import { Pencil, Trash2 } from 'lucide-vue-next'

import { AlertDialogTrigger, AlertDialog } from '@/components/ui/alert-dialog'
import AlertConfirm from '@/components/AlertConfirm.vue';
import AddAdmin from '@/components/AddAdmin.vue';
import { cbFetch } from '@/services/api-service';
import { onMounted, ref } from 'vue';
import { Separator } from '@/components/ui/separator'
import type { Cols, Config } from '@/utils/types';
import Input from '@/components/ui/input/Input.vue';
import { Switch } from '@/components/ui/switch'
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from '@/components/ui/select'
import {
  NumberField,
  NumberFieldContent,
  NumberFieldDecrement,
  NumberFieldIncrement,
  NumberFieldInput,
} from '@/components/ui/number-field'
import { debounce } from '@/lib/utils';

const adminCols = ref<Cols[]>([]);
const adminData = ref<{ [key: string]: string }[]>([]);

const config = ref<Config[]>([]);
const newConfig = ref<Config>({} as Config);


async function queryAdmins() {
    const res = await cbFetch(`/api/admin/admins`)
    adminCols.value = res.columns;
    adminData.value = res.data;
}

async function queryConfig() {
    const res = await cbFetch(`/api/admin/configs`)
    config.value = res.data;
}

const saveConfig = debounce(async (name: string, value: string) => {
    await cbFetch(`/api/admin/configs`, 'PUT', {name,value});
}, 500)

const deleteConfig = async (name: string) => {
    await cbFetch(`/api/admin/configs`, 'DELETE', {name});
    await queryConfig();
}   

const addConfig = async () => {
    await cbFetch(`/api/admin/configs`, 'POST', newConfig.value);
    await queryConfig();
    newConfig.value = {} as Config;
}   

onMounted(async () => {
    await queryAdmins();
    await queryConfig();
})
</script>

<template>
    <div class="settings-container p-4">
        <h2 class="scroll-m-20 border-b pb-2 text-3xl font-semibold tracking-tight transition-colors first:mt-0">
            Configuration
        </h2>
        <p class="leading-7 [&:not(:first-child)]:mb-6">Configure how you app should behave and add your custom configs</p>

        <Table v-if="config && config.length">
            <TableBody>
                <TableRow v-for="conf of config">
                    <TableCell>
                        {{ conf.name }}
                    </TableCell>
                    <TableCell>
                        <Switch v-if="conf.type === 'BOOLEAN'" :checked="conf.value == 'true'" @update:checked="(v) => {conf.value = v+''; saveConfig(conf.name, conf.value)}"/>
                        <NumberField v-else-if="conf.type === 'NUMBER'" :model-value="+conf.value" @update:model-value="(v) => {conf.value = v+''; saveConfig(conf.name, conf.value)}">
                            <NumberFieldContent>
                            <NumberFieldDecrement />
                            <NumberFieldInput />
                            <NumberFieldIncrement />
                            </NumberFieldContent>
                        </NumberField>
                        <Input v-else type="text" :model-value="conf.value" @update:model-value="(v) => {conf.value = v+''; saveConfig(conf.name, conf.value)}"/>
                    </TableCell>
                    <TableCell class="text-center">
                        <Button variant="destructive" @click="deleteConfig(conf.name)">
                            <AlertDialog>
                    <AlertDialogTrigger>
                            <Trash2 />
                        <AlertConfirm :description="`This action will delete the config ${conf.name}`" :action="() => deleteConfig(conf.name)">
                        </AlertConfirm>
                    </AlertDialogTrigger>
                </AlertDialog>
                        </Button>
                    </TableCell>
                </TableRow>
                
            </TableBody>
        </Table>

        <div class="flex gap-3 items-center">
            <Input placeholder="Key" type="text" v-model="newConfig.name"/>
            <Switch v-if="newConfig.type === 'BOOLEAN'" :checked="newConfig.value == 'true'" @update:checked="(v) => newConfig.value = v+''"/>
            <NumberField v-else-if="newConfig.type === 'NUMBER'" :model-value="+newConfig.value" @update:model-value="(v) => newConfig.value = v+''">
                            <NumberFieldContent>
                            <NumberFieldDecrement />
                            <NumberFieldInput />
                            <NumberFieldIncrement />
                            </NumberFieldContent>
                        </NumberField>
                        <Input v-else type="text" :model-value="newConfig.value" @update:model-value="(v) => newConfig.value = v+''"/>
            <Select v-model="newConfig.type">
                <SelectTrigger>
                <SelectValue placeholder="type" />
                </SelectTrigger>
                <SelectContent>
                    <SelectItem value="BOOLEAN">
                        Boolean
                    </SelectItem>
                    <SelectItem value="NUMBER">
                        Number
                    </SelectItem>
                    <SelectItem value="TEXT">
                        Text
                    </SelectItem>
                </SelectContent>
            </Select>
            <Button variant="outline" @click="addConfig">
                Add New Config
            </Button>
        </div>
        


        <h2 class="scroll-m-20 border-b pb-2 text-3xl font-semibold tracking-tight transition-colors mt-8">
            Admins
        </h2>
        <p class="leading-7 [&:not(:first-child)]:mb-6">Create, update and/or delete admins</p>

        <div class="actions">
            <Dialog>
                <DialogTrigger>
                    <Button variant="outline">
                        Add new Admin
                    </Button>
                </DialogTrigger>
                <AddAdmin ref="create-admin"></AddAdmin>
            </Dialog>
        </div>


        <Table v-if="adminCols && adminCols.length">
            <TableHeader>
                <TableRow>
                    <TableHead v-for="col of adminCols">
                        {{ col.name }}
                    </TableHead>
                    <TableHead>
                        Actions
                    </TableHead>
                </TableRow>
            </TableHeader>
            <TableBody>
                <TableRow v-for="d of adminData">
                    <TableCell v-for="col of adminCols">
                        {{ d[col.name] }}
                    </TableCell>
                    <TableCell class="flex gap-1.5 justify-center">
                        <Button variant="secondary">
                            <Pencil />
                        </Button>
                        <Button variant="secondary">
                            <Trash2 />
                        </Button>
                    </TableCell>
                </TableRow>
            </TableBody>
        </Table>
    </div>
</template>

<style scoped>
.logout-btn {
    position: absolute;
    right: 1rem;
    top: 1rem;
    display: flex;
}

.settings-container {
    padding: 1rem;
    display: flex;
    flex-direction: column;
}
</style>